import "./styles/App.css";
import {Toolbar} from "./Toolbar";
import { CodeEditor } from "./Editor";
import { FilesProvider } from "./FileManager";

function App() {

  return (
    <FilesProvider>
      <div className="app-container">
        <Toolbar/>
        <CodeEditor/>
      </div>
    </FilesProvider>
  )
}

export default App;
