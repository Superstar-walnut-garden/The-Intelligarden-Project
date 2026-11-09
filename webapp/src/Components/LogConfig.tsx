import React, { useState } from "react";
import "bootstrap/dist/css/bootstrap.min.css";
import { GearFill } from "react-bootstrap-icons";

export interface LogConfigProps {
  logInterval: number;
  logOnlyOnChange: boolean;
  loggingEnabled: boolean;
}

interface LogConfigComponentProps {
  config: LogConfigProps;
  onChange: (config: LogConfigProps) => void;
  disabled: boolean;
}

const LogConfig: React.FC<LogConfigComponentProps> = ({
  config,
  onChange,
  disabled = false,
}) => {
  const [showModal, setShowModal] = useState(false);
  const [localConfig, setLocalConfig] = useState(config);

  const handleToggleLogging = () => {
    setLocalConfig((prevConfig) => {
      const updatedConfig = {
        ...prevConfig,
        loggingEnabled: !prevConfig.loggingEnabled,
      };
      onChange(updatedConfig);
      return updatedConfig;
    });
  };

  return (
    <div className="d-flex align-items-center m-2">
      <div className="d-flex align-items-center">
        <div className="btn-group" role="group">
          {/* Toggle button */}
          <button
            className={`btn ${
              localConfig.loggingEnabled
                ? "btn-success"
                : "btn-outline-secondary"
            } 
                d-flex align-items-center rounded-end-0`}
            onClick={handleToggleLogging}
            disabled={disabled}
          >
            {localConfig.loggingEnabled ? "Logging On" : "Logging Off"}
          </button>

          {/* Settings button */}
          <button
            className="btn btn-outline-secondary d-flex align-items-center rounded-start-0"
            onClick={() => setShowModal(true)}
            disabled={disabled}
            aria-label="Logging Settings"
          >
            <GearFill />
          </button>
        </div>
      </div>

      {/* Modal for settings */}
      {showModal && (
        <div
          className="modal show d-block"
          tabIndex={-1}
          role="dialog"
          style={{ backgroundColor: "rgba(0,0,0,0.5)" }}
        >
          <div className="modal-dialog" role="document">
            <div className="modal-content">
              <div className="modal-header">
                <h5 className="modal-title">Logging Settings</h5>
                <button
                  type="button"
                  className="btn-close"
                  onClick={() => {
                    setShowModal(false);
                    onChange(localConfig);
                  }}
                ></button>
              </div>
              <div className="modal-body">
                <div className="mb-3">
                  <label className="form-label">Interval (seconds)</label>
                  <input
                    type="number"
                    className="form-control"
                    value={localConfig.logInterval}
                    onChange={(e) => {
                      setLocalConfig((prevItem) => ({
                        ...prevItem,
                        logInterval: Number(e.target.value),
                      }));
                    }}
                  />
                </div>
                <div className="form-check">
                  <input
                    className="form-check-input"
                    type="checkbox"
                    checked={localConfig.logOnlyOnChange}
                    onChange={(e) => {
                      setLocalConfig((prevItem) => ({
                        ...prevItem,
                        logOnlyOnChange: Boolean(e.target.checked),
                      }));
                    }}
                  />
                  <label className="form-check-label">Log only on change</label>
                </div>
              </div>
              <div className="modal-footer">
                <button
                  type="button"
                  className="btn btn-success"
                  onClick={() => {
                    setShowModal(false);
                    onChange(localConfig);
                  }}
                >
                  Apply
                </button>
              </div>
            </div>
          </div>
        </div>
      )}
    </div>
  );
};

export default LogConfig;
