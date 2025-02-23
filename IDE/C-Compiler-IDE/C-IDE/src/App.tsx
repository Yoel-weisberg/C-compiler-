import "./styles/App.css";
import {Toolbar} from "./Toolbar";
import { CodeEditor } from "./Editor";
import {FileProvider} from "./FileEventsHandler";

function App() {
  return (
    <FileProvider>
      <div className="app-container">
        <Toolbar/>
        <CodeEditor/>
      </div>
    </FileProvider>
  )
}

export default App;