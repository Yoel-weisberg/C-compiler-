// import { useEffect } from "react";
// import { useTerminal } from "./TerminalManager"; 
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

// export const TerminalComponent = () => {
//   const { terminalRef, terminalHeight, isTerminalVisible } = useTerminal();
//   const { printToTerminal } = useTerminal(); // Keep this here, but control usage
//   console.log("IN TERMINAL!");
//   // Make sure you're not printing unnecessarily in every render
//   useEffect(() => {
//     if (isTerminalVisible) {
//       printToTerminal("Terminal Ready...");
//     }
//   }, [isTerminalVisible, printToTerminal]); // Only print when terminal is visible

//   return (
//     <div 
//       ref={terminalRef} 
//       className="terminal-container" 
//       style={{ 
//         height: isTerminalVisible ? `${terminalHeight}px` : "0px", 
//         width: "100%", 
//         position: "absolute", 
//         bottom: 0, 
//         overflow: "hidden",
//         backgroundColor: "#ff4305"
//       }} 
//     />
//   );
// };






// import { useEffect } from "react";
// import "./styles/Terminal.css"; 
// import { FitAddon } from '@xterm/addon-fit';
// import { useXTerm } from 'react-xtermjs';

// export const TerminalComponent = () => {
//   const { instance, ref } = useXTerm()
//   const fitAddon = new FitAddon()

//   useEffect(() => {
//     // Load the fit addon
//     instance?.loadAddon(fitAddon)

//     const handleResize = () => fitAddon.fit()

//     // Write custom message on your terminal
//     instance?.writeln('Welcome react-xtermjs!')
//     instance?.writeln('This is a simple example using an addon.')

//     // Handle resize event
//     window.addEventListener('resize', handleResize)
//     return () => {
//       window.removeEventListener('resize', handleResize)
//     }
//   }, [ref, instance])

//   return <div ref={ref} style={{ height: '100%', width: '100%' }} />
// }


// function useXTerm(): { instance: any; ref: any; } {
//     throw new Error("Function not implemented.");
// }
// export const ConsoleTerminal: React.FC = () => {
//     const terminalRef = useRef<HTMLDivElement>(null);
//     const term = useRef<Terminal | null>(null);
//     const { isTerminalOpen, terminalOutput } = useTerminal(); // Get shared state
//     const { terminalHeight } = useSizeContext();   

//     // Initialize terminal when opened
//     useEffect(() => {
//         if (isTerminalOpen && terminalRef.current) {
//             if (!term.current) {
//                 term.current = new Terminal({
//                     theme: { background: "#1e1e1e", foreground: "#ffffff" },
//                     cursorBlink: true,
                    
//                     // fontFamily: "'Cascadia Code', sans-serif",
//                     // fontSize: 12,
//                 });
//                 term.current.open(terminalRef.current);
//                 term.current.writeln("Terminal Ready...");
//             }
//         }

//         return () => {
//             term.current?.dispose();
//             term.current = null; // Cleanup on unmount
//         };
//     }, [isTerminalOpen]);

//     // Listen for terminal output updates and write only the new message
//     useEffect(() => {
//         if (!term.current) return; // Ensure terminal is initialized

//         if (terminalOutput.length > 0) {
//             const lastMessage = terminalOutput[terminalOutput.length - 1]; // Get last message
//             console.log("Writing to terminal:", lastMessage); // Debugging log
//             term.current.writeln(lastMessage);
//             term.current.scrollToBottom(); // Ensure latest output is visible
//         }
//     }, [terminalOutput]);

//     if (!isTerminalOpen) return null; // Hide if terminal is closed

//     return (
//         <div 
//             ref={terminalRef}
//             className="terminal-container" 
//             style={{ height: `${terminalHeight}px`, overflow: "visible" }} 
//         />
//     );
// };

