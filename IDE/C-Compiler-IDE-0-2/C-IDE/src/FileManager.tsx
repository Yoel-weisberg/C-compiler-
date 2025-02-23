import { useRef } from 'react';

export const GetFile = () => {
    const fileInputRef = useRef<HTMLInputElement>(null);

    const openFileExplorer = () => {
        if (fileInputRef.current) {
          fileInputRef.current.click(); // Trigger the file input click programmatically
        }
      };
    
      const handleFileChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        const files = event.target.files;
        if (files) {
          console.log(files); // Logs the selected FileList
          return files;
        }
        return null;
      };

      return (
        <div>
          <button onClick={openFileExplorer}>Open File Explorer</button>
          <input
            ref={fileInputRef}
            type="file"
            style={{ display: 'none' }} // Hide the file input
            onChange={handleFileChange} // Handle the file selection
          />
        </div>
      );
}