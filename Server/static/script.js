document.addEventListener('DOMContentLoaded', () => {
    const socket = io();

    document.getElementById('send-brightness').addEventListener('click', () => {
        const brightness = document.getElementById('brightness-input').value;
        socket.emit('setBrightness', { brightness });
    });

    document.getElementById('send-level').addEventListener('click', () => {
        const level = document.getElementById('level-input').value;
        socket.emit('setLevel', { level });
    });
});