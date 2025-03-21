// import { createContext, useContext, useRef, useState, useEffect } from "react";
// import { FitAddon } from "@xterm/addon-fit";
// import { Terminal } from "xterm";

// interface TerminalContextType {
//   terminalRef: React.RefObject<HTMLDivElement>;
//   terminalInstance: Terminal | null;
//   toggleTerminal: () => void;
//   printToTerminal: (message: string) => void;
//   terminalHeight: number;
//   isTerminalVisible: boolean;
// }

// // Create Context
// const TerminalContext = createContext<TerminalContextType | null>(null);

// // Provider Component
// export const TerminalProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
//   const terminalRef = useRef<HTMLDivElement>(null);
//   const [terminalInstance, setTerminalInstance] = useState<Terminal | null>(null);
//   const [isTerminalVisible, setIsTerminalVisible] = useState(false);
//   const [terminalHeight, setTerminalHeight] = useState(0); // Start hidden

//   useEffect(() => {
//     if (!terminalRef.current || terminalInstance) return; // Prevent duplicate initialization
  
//     const newTerm = new Terminal({
//       theme: { background: "#1e1e1e", foreground: "#ffffff" },
//       cursorBlink: false,
//       fontSize: 12,
//     });
  
//     const fitAddon = new FitAddon();
//     newTerm.loadAddon(fitAddon);
//     newTerm.open(terminalRef.current);
  
//     setTimeout(() => {
//       fitAddon.fit();
//       newTerm.writeln("Terminal Ready...");
//     }, 100);
  
//     setTerminalInstance(newTerm); // Set terminal instance only once
  
//     const handleResize = () => fitAddon.fit();
//     window.addEventListener("resize", handleResize);
  
//     return () => {
//       newTerm.dispose();
//       window.removeEventListener("resize", handleResize);
//     };
//   }, []); // Empty dependency array to only run once on mount

//   // Function to toggle terminal visibility
//   const toggleTerminal = () => {
//     setIsTerminalVisible((prev) => {
//       const newVisibility = !prev;
  
//       if (newVisibility && terminalInstance) {
//         setTimeout(() => {
//           terminalInstance.clear(); // Clear the previous content if needed
//           terminalInstance.writeln("Terminal Ready...");
//         }, 100); // Give a small delay to ensure it's visible before writing
//       }
  
//       return newVisibility;
//     });
  
//     setTerminalHeight((prev) => (prev === 0 ? 200 : 0)); // Show (200px) or Hide (0px)
//   };

//   return (
//     <TerminalContext.Provider value={{ terminalRef, terminalInstance, toggleTerminal, printToTerminal: (msg) => terminalInstance?.writeln(msg), terminalHeight, isTerminalVisible }}>
//       {children}
//     </TerminalContext.Provider>
//   );
// };

// // Custom Hook to Access Terminal Context
// export const useTerminal = () => {
//   const context = useContext(TerminalContext);
//   if (!context) {
//     throw new Error("useTerminal must be used within a TerminalProvider");
//   }
//   return context;
// };













































































// import React, { createContext, useState, useContext, ReactNode } from "react";

// interface TerminalContextType {
//   isTerminalOpen: boolean;
//   toggleTerminal: () => void;
//   terminalOutput: string[];
//   printToTerminal: (message: string) => void;
// }

// const TerminalContext = createContext<TerminalContextType | undefined>(undefined);

// export const TerminalProvider: React.FC<{ children: ReactNode }> = ({ children }) => {
//   const [isTerminalOpen, setIsTerminalOpen] = useState(false);
//   const [terminalOutput, setTerminalOutput] = useState<string[]>([]);

//   const toggleTerminal = () => setIsTerminalOpen((prev) => !prev);

//   const printToTerminal = (message: string) => {
//     setTerminalOutput((prevOutput) => [...prevOutput, message]);
//   };

//   return (
//     <TerminalContext.Provider value={{ isTerminalOpen, toggleTerminal, terminalOutput, printToTerminal }}>
//       {children}
//     </TerminalContext.Provider>
//   );
// };

// // Custom hook for easy access
// export const useTerminal = () => {
//   const context = useContext(TerminalContext);
//   if (!context) {
//     throw new Error("useTerminal must be used within a TerminalProvider");
//   }
//   return context;
// };

