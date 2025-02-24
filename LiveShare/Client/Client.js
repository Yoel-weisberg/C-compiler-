
const RequestCodes = {
  INITIAL_FETCH: 100,
  FETCH_FILE: 101,
  UPDATE: 102
};

const ResponseCodes = {
  INITIAL_FETCH: 200,
  FETCH_FILE: 201,
  CHANGE_DETECTED: 202
};

export default class Client {
  constructor(peerId) {
    this.peerId = peerId;
    this.peer = new Peer();
    this.conn = this.peer.connect(this.peerId);
    this.counter = 0;

    // Create a reference to the display element
    this.counterElement = document.getElementById("counter-display");

    this.conn.on("open", () => {
      console.log("Connected to peer:", this.peerId);
      // Receive messages
      this.conn.on("data", (data) => {
        const packetCode = data.PacketCode;
        switch (packetCode) {
          case ResponseCodes.INITIAL_FETCH:
            console.log("Received INITIAL_FETCH");
            this.updateCounter(data.data);
            break;
          case ResponseCodes.FETCH_FILE:
            console.log("Received FETCH_FILE");
            break;
          case ResponseCodes.CHANGE_DETECTED:
            console.log("Received CHANGE_DETECTED, change: ", data.data);
            this.updateCounter(data.data);
            break;
          case ResponseCodes.FILE_UPDATED:
          default:
        }
      });
    });
  }

  updateCounter(newValue) {
    this.counter = newValue;
    if (this.counterElement) {
      this.counterElement.textContent = `Counter: ${this.counter}`;
    }
  }
}
