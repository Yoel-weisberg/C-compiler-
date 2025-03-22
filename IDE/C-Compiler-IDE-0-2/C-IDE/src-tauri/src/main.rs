// Prevents additional console window on Windows in release, DO NOT REMOVE!!
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]
use tauri::Manager; 
// --- For File explorer --- 
use std::fs;
use std::path::Path;
use tauri::command;
use serde::{Serialize, Deserialize};

// File Explorer related
#[derive(Serialize, Deserialize)]
struct FileNode {
    name: String,
    path: String,
    is_directory: bool,
    children: Option<Vec<FileNode>>, // Optional children for folders
}

#[command]
fn read_directory(path: String) -> Vec<FileNode> {
    let mut entries = vec![];
    if let Ok(dir) = fs::read_dir(Path::new(&path)) {
        for entry in dir.flatten() {
            let path = entry.path();
            let file_name = entry.file_name().into_string().unwrap_or_default();
            let is_directory = path.is_dir();

            entries.push(FileNode {
                name: file_name,
                path: path.to_string_lossy().into_owned(),
                is_directory,
                children: if is_directory {
                    Some(read_directory(path.to_string_lossy().into_owned()))
                } else {
                    None
                },
            });
        }
    }
    entries
}



// --- Compiling ---
#[tauri::command]
fn run_exe(file_path: String) -> Result<String, String> {
    use std::process::Command;

    let output = Command::new("../FinalProjectYoelAndNeta.exe")
        .arg(file_path)
        .output(); // Capture both stdout and stderr

    match output {
        Ok(output) => {
            let stdout = String::from_utf8_lossy(&output.stdout); // Capture stdout
            let stderr = String::from_utf8_lossy(&output.stderr); // Capture stderr

            if output.status.success() {
                // Return both stdout and stderr, if you want to see them
                Ok(format!("stdout: {}", stdout))
                //Ok("Executed successfully".to_string())
            } else {
                // If execution fails, return stderr
                Err(format!("Failed to execute: {}", stderr))
            }
        }
        Err(e) => Err(e.to_string()), // Error in running the process
    }
}





fn main() {
    tauri::Builder::default()
        .setup(|app| {
            #[cfg(debug_assertions)] // only include this code on debug builds
            {
                let window = app.get_webview_window("main").unwrap();
                window.open_devtools();
            }
            Ok(())
        })
        .invoke_handler(tauri::generate_handler![run_exe, read_directory])
        .plugin(tauri_plugin_fs::init())
        .plugin(tauri_plugin_dialog::init())
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}


// // Prevents additional console window on Windows in release, DO NOT REMOVE!!
// #![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

// fn main() {
//     tauri::Builder::default()
//         .plugin(tauri_plugin_fs::init())
//         .plugin(tauri_plugin_dialog::init())
//         .run(tauri::generate_context!())
//         .expect("error while running tauri application");
// }

