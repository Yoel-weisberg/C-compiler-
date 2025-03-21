// import React, { createContext, useContext, useState } from "react";

// interface SizeContextType {
//   terminalHeight: number;
//   setTerminalHeight: (height: number) => void;
//   editorHeight: number;
//   setEditorHeight: (height: number) => void;
//   // Not Implemented
//   fileExplorerWidth: number;
//   setFileExplorerWidth: (width: number) => void;
// }

// const SizeContext = createContext<SizeContextType | undefined>(undefined);

// export const SizeProvider = ({ children }: { children: React.ReactNode }) => {
//   const [terminalHeight, setTerminalHeight] = useState(0); // Application starts without a terminal
//   const [editorHeight, setEditorHeight] = useState(window.innerHeight);
//   const [fileExplorerWidth, setFileExplorerWidth] = useState(300);

//   return (
//     <SizeContext.Provider
//       value={{
//         terminalHeight,
//         setTerminalHeight,
//         editorHeight,
//         setEditorHeight,
//         fileExplorerWidth,
//         setFileExplorerWidth,
//       }}
//     >
//       {children}
//     </SizeContext.Provider>
//   );
// };

// // Custom hook for accessing the context
// export const useSizeContext = (): SizeContextType => {
//   const context = useContext(SizeContext);
//   if (!context) {
//     throw new Error("useSizeContext must be used within a SizeProvider");
//   }
//   return context;
// };
