import "./styles/App.css";
import {Toolbar} from "./Toolbar";
import { CodeEditor } from "./Editor";
import { FilesProvider } from "./FileManager";
import { Tabs } from "./Tabs";
import { TerminalComponent } from "./Terminal";
import { TerminalProvider } from "./TerminalProvider";

function App() {

  return (
    <TerminalProvider>
        <FilesProvider> 
            <div className="app-container">
              <Toolbar/>
              <Tabs/>
              <CodeEditor/>
              <TerminalComponent/>
            </div>
        </FilesProvider>
      </TerminalProvider>
  )
}

export default App;


// function App() {

//   return (
//     // <TerminalProvider>
//         <FilesProvider> 
//             <div className="app-container">
//               <Toolbar/>
//               <Tabs/>
//               <CodeEditor/>
//               <TerminalComponent/>
//             </div>
//         </FilesProvider>
//     // </TerminalProvider>
//   )
// }

// export default App;


/*
function App() {

  return (
    //<SizeProvider> 
      <FilesProvider> 
        <TerminalProvider>
          <div className="app-container">
            <Toolbar/>
            <Tabs/>
            <CodeEditor/>
            //<ConsoleTerminal/>
          </div>
        </TerminalProvider>
      </FilesProvider>
    //</SizeProvider>
  )
}

export default App;
*/ 