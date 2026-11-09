import React, { useEffect, useState } from "react";
import TitleBar from "../Components/TitleBar";
import * as ApiService from "../api/apiService";
import "bootstrap/dist/css/bootstrap.min.css";
import { Tab, Tabs } from "react-bootstrap";
import LogFilesPage from "./LogFilesPage";

const DataLoggingPage: React.FC = () => {
  const [logConfig, setLogConfig] =
    useState<ApiService.DataLoggingConfigApiData | null>(null);

  const [logFiles, setLogFiles] = useState<ApiService.LogFilesResponse | null>(
    null
  );

  const fetchLogFiles = async () => {
    try {
      const logFiles = await ApiService.getLogFiles();
      setLogFiles(logFiles);
    } catch (error) {
      console.error("Error fetching log files:", error);
    }
  };

  const fetchConfig = async () => {
    try {
      const config = await ApiService.getDataLoggingConfig();
      setLogConfig(config);
    } catch (error) {
      console.error("Error fetching Data Logging config:", error);
    }
  };
  // retrive config from api
  useEffect(() => {
    fetchConfig();
  }, []);

  useEffect(() => {
    fetchLogFiles();
  }, []);

  // save config to api
  const saveConfig = async (config: ApiService.DataLoggingConfigApiData) => {
    try {
      await ApiService.setDataLoggingConfig(config);
    } catch (error) {
      console.error("Error saving Data Logging config:", error);
    }
    fetchConfig();
  };

  return (
    <>
      <TitleBar title="Data Logging" />
      <div className="container mt-4">
        <div className="bg-light p-4 rounded border w-100 w-md-50 mx-auto">
          <Tabs defaultActiveKey="config" className="mb-3">
            <Tab eventKey="config" title="Config">
              {logConfig ? (
                <form
                  onSubmit={(e) => {
                    e.preventDefault();
                    saveConfig(logConfig);
                  }}
                >
                  <div className="form-check form-switch mb-3">
                    <input
                      className="form-check-input"
                      type="checkbox"
                      id="enabled"
                      checked={logConfig?.enabled}
                      onChange={(e) =>
                        setLogConfig((prev) =>
                          prev ? { ...prev, enabled: e.target.checked } : prev
                        )
                      }
                    />
                    <label className="form-check-label" htmlFor="enabled">
                      Logging Enabled
                    </label>
                  </div>

                  <div className="mb-3">
                    <label htmlFor="maxFileSizeBytes" className="form-label">
                      Max File Size (bytes)
                    </label>
                    <input
                      type="number"
                      className="form-control"
                      id="maxFileSizeBytes"
                      placeholder="Enter maximum size"
                      value={logConfig?.maxFileSizeBytes}
                      onChange={(e) =>
                        setLogConfig((prev) =>
                          prev
                            ? {
                                ...prev,
                                maxFileSizeBytes: Number(e.target.value),
                              }
                            : prev
                        )
                      }
                    />
                  </div>

                  <div className="mb-3">
                    <label
                      htmlFor="maxFileRotationCount"
                      className="form-label"
                    >
                      Max File Rotation Count
                    </label>
                    <input
                      type="number"
                      className="form-control"
                      id="maxFileRotationCount"
                      placeholder="Enter rotation count"
                      value={logConfig?.maxFileRotationCount}
                      onChange={(e) =>
                        setLogConfig((prev) =>
                          prev
                            ? {
                                ...prev,
                                maxFileRotationCount: Number(e.target.value),
                              }
                            : prev
                        )
                      }
                    />
                  </div>

                  <div className="mb-3">
                    <label htmlFor="basePath" className="form-label">
                      Base Path
                    </label>
                    <input
                      type="text"
                      className="form-control"
                      id="basePath"
                      placeholder="Enter base path"
                      value={logConfig?.basePath}
                      onChange={(e) =>
                        setLogConfig((prev) =>
                          prev ? { ...prev, basePath: e.target.value } : prev
                        )
                      }
                    />
                  </div>

                  <div className="mb-3">
                    <label htmlFor="status" className="form-label">
                      Status
                    </label>
                    <input
                      type="text"
                      className="form-control"
                      id="status"
                      placeholder="Current status"
                      disabled
                      value={logConfig?.status}
                    />
                  </div>

                  <button
                    type="submit"
                    className="btn btn-primary"
                    onClick={() => logConfig && saveConfig(logConfig)}
                  >
                    Save / Apply
                  </button>
                </form>
              ) : (
                <div>Loading...</div>
              )}
            </Tab>
            <Tab eventKey="raw" title="Raw Data">
              {logFiles ? (
                <LogFilesPage logFilesData={logFiles} />
              ) : (
                <div>Loading...</div>
              )}
            </Tab>
            <Tab eventKey="charts" title="Charts & Analysis">
              {/* Charts content */}
            </Tab>
          </Tabs>
        </div>
      </div>
    </>
  );
};

export default DataLoggingPage;
