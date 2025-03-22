import Server from "./Server.js";

window.addEventListener('load', () => {
    const server = new Server();
    
    server.peer.on('open', (id) => {
        const peerIdElement = document.getElementById('peerID');
        
        if (peerIdElement) {
            peerIdElement.textContent = "Your peer ID is: " + id;
            // Try both textContent and value
        } else {
            console.error("Element with ID 'peerID' not found");
        }

        const counterButton = document.getElementById('counter');
        counterButton.addEventListener('click', () => {
            const newVal = server.incrementCounter();
            counterButton.textContent = newVal;
        });
    });
});