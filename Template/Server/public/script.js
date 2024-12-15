document.addEventListener("DOMContentLoaded", () => {
  const socket = io();

  // Log messages
  const log = (message) => {
    const logDiv = document.getElementById("log");
    const newLog = document.createElement("p");
    newLog.textContent = message;
    logDiv.appendChild(newLog);
  };

  // Connection events
  socket.on("connect", () => log("Connected to server."));
  socket.on("disconnect", () => log("Disconnected from server."));
  socket.on("message", (msg) => log(`Received message: ${msg}`));

  // Send delay
  document.getElementById("send-delay").addEventListener("click", () => {
    const delay = document.getElementById("delay-input").value;
    if (delay) {
      socket.emit("setDelayTime", { delay: parseInt(delay) });
      log(`Sent delay: ${delay}`);
    } else {
      log("Please enter a valid delay value.");
    }
  });

  // Send parameter
  document.getElementById("send-parameter").addEventListener("click", () => {
    const parameter = document.getElementById("parameter-input").value;
    if (parameter) {
      socket.emit("setParameter", { parameter });
      log(`Sent parameter: ${parameter}`);
    } else {
      log("Please enter a valid parameter.");
    }
  });

  // Send brightness
  document.getElementById("send-brightness").addEventListener("click", () => {
    const brightness = document.getElementById("brightness-input").value;
    if (brightness >= 0 && brightness <= 255) {
      socket.emit("setBrightness", { brightness: parseInt(brightness) });
      log(`Sent brightness: ${brightness}`);
    } else {
      log("Please enter a brightness value between 0 and 255.");
    }
  });

  // Send level
  document.getElementById("send-level").addEventListener("click", () => {
    const level = document.getElementById("level-input").value;
    if (level) {
      socket.emit("setLevel", { level: parseInt(level) });
      log(`Sent level: ${level}`);
    } else {
      log("Please enter a valid level value.");
    }
  });
});
