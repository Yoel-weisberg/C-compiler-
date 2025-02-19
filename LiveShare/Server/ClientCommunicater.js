export default class ClientCommunicater {
    constructor(conn, server) {
        this.conn = conn;
        this.server = server;
        this.pendingChnages = new Array();
    }
}