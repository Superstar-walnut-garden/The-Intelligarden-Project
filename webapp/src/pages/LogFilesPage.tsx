import React, { useState } from "react";
import FileBrowser from "../Components/FileBrowser";
import FileViewer from "../Components/FileViewer";
import { LogFilesResponse } from "../api/apiService";

interface LogFilesPageProps {
  logFilesData: LogFilesResponse;
}

const LogFilesPage: React.FC<LogFilesPageProps> = ({ logFilesData }) => {
  const [selectedFilePath, setSelectedFilePath] = useState<string | null>(null);
  const [viewerOpen, setViewerOpen] = useState(false);

  return (
    <div>
      <FileBrowser
        data={logFilesData}
        onFileClick={(path) => {
          setSelectedFilePath(path);
          setViewerOpen(true);
        }}
      />

      <FileViewer
        filePath={selectedFilePath}
        show={viewerOpen}
        onClose={() => setViewerOpen(false)}
      />
    </div>
  );
};

export default LogFilesPage;
