import {Peer} from "https://esm.sh/peerjs@1.5.4?bundle-deps"

const RequestCodes = {
    INITIAL_FETCH: 100,
    FETCH_FILE: 101,
    UPDATE: 102
}

const ResponseCodes = {
    INITIAL_FETCH: 200,
    FETCH_FILE: 201,
    CHANGE_DETECTED: 202
}

export {RequestCodes, ResponseCodes}

export default class Server {
    constructor() {
        this.peer = new Peer();
        this.peerId = null;
        this.areThereChnages = false;
        this.currentChange = null;
        this.connections = new Set();  // Fixed: moved from const to this.
        this.counter = 0;
        this.peer.on('open', (id) => {
            this.peerId = id;
            this.mainLoop();
        });
    } 

    incrementCounter() {
        this.counter++;
        this.broadcost(this.counter);
        return this.counter;
    }
    getPeerId() {
        return this.peerId;
    }

    mainLoop() {
        this.peer.on('connection', (conn) => {
            console.log('New connection:', conn.peer);
            
            //firstly we are going to send the user the basic information and then add him 
            // to the broadcost list
            conn.on('open', () => {
                conn.send(this.makeResponse(ResponseCodes.INITIAL_FETCH, this.getInitialMessage()));
                conn.send(this.makeResponse(ResponseCodes.INITIAL_FETCH, this.getFileContent()));
                // finally we are adding the connection to the broadcost list
                this.connections.add(conn);
            });

            conn.on('data', (data) => {
                console.log('Received data:', data);
                // handles a case when the client sends us data - which should never happen
            });

            conn.on('close', () => {
                this.connections.delete(conn);
                console.log('Connection closed:', conn.peer);
            });
        });
    }

    broadcost(change) {
        for (const conn of this.connections) {
            if (conn.open) {
                conn.send(this.makeResponse(ResponseCodes.CHANGE_DETECTED, change));
            }
        }
    }

    getInitialMessage() {
        // return the initial message containing the list of files and the current file data
        // return {
        //     files: [],  // Add your file list here
        //     currentFile: null  // Add current file data here
        // };
        return {
            number: this.counter
        }
    }

    getFileContent() {
        return "some file content";
    }

    makeResponse(ResponseCode, ResponseJSON) {
        return {PacketCode: ResponseCode, data: ResponseJSON};
    }

    getCurrentMouseLocation() {
        // Implement mouse location tracking here
        return JSON.stringify({ x: 0, y: 0 });  // Placeholder
    }

    monitorChangesOnMonaco() {
        // Implement Monaco editor change monitoring here
    }
}