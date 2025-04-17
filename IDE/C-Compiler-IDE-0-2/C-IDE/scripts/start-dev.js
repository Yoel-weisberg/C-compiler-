const { execSync } = require("child_process");
const detectPort = require("detect-port");

async function start() {
  const defaultPort = 1420;
  const availablePort = await detectPort(defaultPort);

  console.log(`Starting Vite & Tauri on port ${availablePort}...`);

  execSync(`VITE_PORT=${availablePort} pnpm dev:vite & VITE_PORT=${availablePort} pnpm dev:tauri`, {
    stdio: "inherit",
    shell: true
  });
}

start();
