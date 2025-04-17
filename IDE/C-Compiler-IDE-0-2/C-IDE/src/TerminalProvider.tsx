import React, { createContext, useContext, useRef, useState, useEffect, ReactNode } from 'react';
import { Terminal } from 'xterm';
// import { FitAddon } from '@xterm/addon-fit';

// Define the context type
interface TerminalContextType {
    terminalRef: React.RefObject<Terminal | null>;
    output: string;
    setOutput: React.Dispatch<React.SetStateAction<string>>;
    toggleTerminal: () => void;
    terminalHeight: number;
    isTerminalVisible: boolean;
    writeToTerminal: (str: String) => boolean;
}

// Create context with an initial value of `null`, but ensure proper typing
const TerminalContext = createContext<TerminalContextType | null>(null);

// Add the type for children to the props
interface TerminalProviderProps {
    children: ReactNode;
}

export const TerminalProvider: React.FC<TerminalProviderProps> = ({ children }) => {
    const terminalRef = useRef<Terminal | null>(null);
    const [output, setOutput] = useState<string>('');
    const [isTerminalVisible, setIsTerminalVisible] = useState<boolean>(false); // Track visibility state
    const [terminalHeight, setTerminalHeight] = useState<number>(0); // Start hidden

    useEffect(() => {
        console.log("IN TERMINAL ===> USE_EFFECT");
        terminalRef.current = new Terminal({
            cursorBlink: true,
            theme: {
                background: '#1E1E1E', // VS Code terminal background
                foreground: '#D4D4D4', // VS Code text color
                cursor: '#FFFFFF', // Cursor color
            },
            fontFamily: "'Cascadia Code', monospace",
            fontSize: 14,
        });

        const terminalElement = document.getElementById('terminal');
        if (terminalElement) {
            terminalRef.current.open(terminalElement);
            terminalRef.current.write( 'Welcome to the C-IDE terminal :)\r\n');
        }

        return () => {
            terminalRef.current?.dispose();
        };
    }, []);

    // Function to toggle terminal visibility
    const toggleTerminal = () => {
        console.log("Terminal Height: " + terminalHeight);
        setIsTerminalVisible((prev) => !prev);
        setTerminalHeight((prev) => (prev === 0 ? 200 : 0)); // Show (200px) or Hide (0px)

        setTimeout(() => {
            const scrollArea = document.querySelector('.xterm-scroll-area');
            if (scrollArea) {
                scrollArea.setAttribute("style", "height: 100% !important");
                console.log("Reset xterm-scroll-area height");
            }
        }, 50);
    };

    const writeToTerminal = (str: String): boolean => {
        if (terminalRef.current) {
            terminalRef.current.write(str + '\r\n'); // Write to the terminal
            return true;
        }
        return false; // Return false if terminal is not initialized
    };

    return (
        <TerminalContext.Provider value={{ terminalRef, output, setOutput, isTerminalVisible, toggleTerminal, terminalHeight, writeToTerminal }}>
        {children}
        </TerminalContext.Provider>
    );
};

// Custom hook to access the terminal context
export const useTerminal = () => {
  const context = useContext(TerminalContext);
  if (!context) {
    throw new Error('useTerminal must be used within a TerminalProvider');
  }
//   console.log("Terminal context:", context);
  return context;
};