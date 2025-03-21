import { FileState } from "./FileManager";
import { invoke } from '@tauri-apps/api/core'


export const findFileToCompile = async (getCurrentFile: () => FileState | null): Promise<string> => {
    console.log("Getting currently viewed file");

    const current: FileState | null = getCurrentFile();
    console.log("Current File is: " + current?.lastSavedVersion.name);

    // Check correct file type (".c")
    console.log("Checking for correct file Type");
    if (current?.lastSavedVersion.type !== 'text/x-c') {
        console.error("Incorrect file type: " + current?.lastSavedVersion.type);
        return "Error: Incorrect file type"; // Return an error message
    }

    // Compile program and return the result as a string
    return await compileProgram(current?.location);
};

async function compileProgram(filePath: string | undefined): Promise<string> {
    console.log("Compiling: " + filePath);
    try {
        const result = await invoke('run_exe', { filePath }); // Wait for the result
        return String(result); // Convert the result to a string and return
    } catch (error) {
        console.error("Failed to run executable: ", error);
        return "Error: Failed to run executable"; // Return an error message
    }
}


// export const findFileToCompile = (getCurrentFile: () => FileState | null) => {
//     // Get currently viewed file
//     console.log("Getting currently viewed file");
//     //const {getCurrentFile} = useFilesContext();
//     const current: FileState | null = getCurrentFile();
//     console.log("Current File is: " + current?.lastSavedVersion.name)

//     // Check correct file type (".c")
//     console.log("Checking for correct file Type");
//     if(current?.lastSavedVersion.type != 'text/x-c'){
//         // Pop error window
//         console.error("Incorrect file type: " + current?.lastSavedVersion.type);
        
//     }

//     // compile program
//     return compileProgram(current?.location)
// }


// function compileProgram(filePath: String | undefined) {
//     //const terminalRef = useRef<{ writeToTerminal: (content: string) => void } | null>(null);
//     console.log("Compiling: " + filePath)
//     try {

//         return invoke('run_exe', {filePath});
//         // console.log(result);
//         // terminalRef.current?.writeToTerminal(result.finally.toString());
//     } catch (error) {
//         console.error("Failed to run executable: ", error)
//     }
    
// }


