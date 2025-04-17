import React from "react";
import "./styles/Terminal.css"; 


import { useTerminal } from './TerminalProvider';

export const TerminalComponent: React.FC = () => {
  const { terminalHeight, toggleTerminal } = useTerminal();
  console.log("IN TERMINAL ===> TerminalComponent");

  return (
      <div className="terminal-wrapper">
          {/* Terminal Header (Like VS Code) */}
          <div className="terminal-header">
              <span>TERMINAL</span>
              <button className="terminal-toggle-btn" onClick={toggleTerminal}>
                  {terminalHeight === 0 ? 'Show' : 'Hide'}
              </button>
          </div>

          {/* Terminal Window */}
          <div
              id="terminal"
              className="terminal-container"
              style={{
                  height: '${terminalHeight}px',
                  display: terminalHeight === 0 ? 'none' : 'block',
                  maxHeight: "200px"
              }}
          />
      </div>
  );
};

export default React.memo(TerminalComponent);




