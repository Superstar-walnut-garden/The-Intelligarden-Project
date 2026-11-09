import React, { useState } from "react";
import { LogFilesResponse, LogFolder, LogFile } from "../api/apiService";

interface FileBrowserProps {
  data: LogFilesResponse;
  onFileClick: (filePath: string) => void;
}

const FileBrowser: React.FC<FileBrowserProps> = ({ data, onFileClick }) => {
  return (
    <div className="list-group">
      <FolderView
        folder={{
          name: data.path,
          path: data.path,
          folders: data.folders,
          files: data.files,
        }}
        onFileClick={onFileClick}
      />
    </div>
  );
};

interface FolderViewProps {
  folder: LogFolder;
  onFileClick: (filePath: string) => void;
}

const FolderView: React.FC<FolderViewProps> = ({ folder, onFileClick }) => {
  const [open, setOpen] = useState(false);

  return (
    <div className="mb-2">
      <button
        className="list-group-item list-group-item-action d-flex justify-content-between align-items-center"
        onClick={() => setOpen(!open)}
      >
        <span>📁 {folder.name}</span>
        <span className="badge bg-secondary">{open ? "−" : "+"}</span>
      </button>

      {open && (
        <div className="ms-3 mt-2">
          {folder.folders.map((subFolder) => (
            <FolderView
              key={subFolder.path}
              folder={subFolder}
              onFileClick={onFileClick}
            />
          ))}
          {folder.files.map((file: LogFile) => (
            <button
              key={file.path}
              className="list-group-item list-group-item-action"
              onClick={() => onFileClick(file.path)}
            >
              📄 {file.name}{" "}
              <span className="text-muted">({file.size} bytes)</span>
            </button>
          ))}
        </div>
      )}
    </div>
  );
};

export default FileBrowser;
