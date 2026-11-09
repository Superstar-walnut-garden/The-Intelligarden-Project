import React, { useEffect, useState } from "react";
import { Modal, Button } from "react-bootstrap";
import { getLogFile, LogFileContent } from "../api/apiService";

interface FileViewerProps {
  filePath: string | null; // path of file to open
  show: boolean; // modal visibility
  onClose: () => void; // close callback
}

const FileViewer: React.FC<FileViewerProps> = ({ filePath, show, onClose }) => {
  const [content, setContent] = useState<LogFileContent | null>(null);

  useEffect(() => {
    if (filePath && show) {
      const fetchData = async () => {
        try {
          const data = await getLogFile(filePath);
          setContent(data);
        } catch (err) {
          console.error("Error fetching file:", err);
        }
      };
      fetchData();
    }
  }, [filePath, show]);

  return (
    <Modal show={show} onHide={onClose} size="lg" scrollable>
      <Modal.Header closeButton>
        <Modal.Title>File Viewer</Modal.Title>
      </Modal.Header>
      <Modal.Body style={{ maxHeight: "60vh", overflowY: "auto" }}>
        {content ? (
          <div className="list-group">
            {Object.entries(content).map(([timestamp, params]) => (
              <div key={timestamp} className="list-group-item">
                <strong>{timestamp}</strong>
                <ul className="mb-0">
                  {Object.entries(params).map(([key, value]) => (
                    <li key={key}>
                      {key}: {String(value)}
                    </li>
                  ))}
                </ul>
              </div>
            ))}
          </div>
        ) : (
          <p>Loading file contents...</p>
        )}
      </Modal.Body>
      <Modal.Footer>
        <Button variant="secondary" onClick={onClose}>
          Close
        </Button>
      </Modal.Footer>
    </Modal>
  );
};

export default FileViewer;
