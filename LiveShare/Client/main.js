import Client from "./Client.js";


window.addEventListener('load', () => {
    console.log("Page is loaded");
    function handleSubmit(event) {
        event.preventDefault(); // Prevent page reload
        let inputValue = document.getElementById("inputBox").value;
        const client = new Client(inputValue);
    }
    // Add event listener to form submit
    const form = document.getElementById('form');
    form.addEventListener('submit', handleSubmit);
});
