import React, { useState, useEffect } from "react";
import "bootstrap/dist/css/bootstrap.min.css";
import { PencilSquare } from "react-bootstrap-icons";
import { FloppyFill } from "react-bootstrap-icons";
import { FusionBusApiData, VentDriveApiData } from "../api/apiService";
import ItemTitle from "./ItemTitle";
import LogConfig from "./LogConfig";
import { LogConfigProps } from "./LogConfig";
import { Button, ProgressBar, Spinner } from "react-bootstrap";
import SensorPicker from "./SensorPicker";

interface TempSensorItemComponentProps {
  item: VentDriveApiData;
  devices: FusionBusApiData[];
  onRemove: (id: number) => void;
  onSave: (item: VentDriveApiData) => void;
}

const VentDriveItem: React.FC<TempSensorItemComponentProps> = ({
  item,
  devices,
  onRemove,
  onSave,
}) => {
  const encode64BitNumberToBase62 = (num: number) => {
    const BASE62 =
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    let bigIntNum = BigInt(num);
    bigIntNum = (bigIntNum >> 8n) & ((1n << 48n) - 1n);

    let base62String = "";
    while (bigIntNum > 0) {
      let remainder = bigIntNum % 62n;
      base62String = BASE62[Number(remainder)] + base62String;
      bigIntNum /= 62n;
    }
    return base62String;
  };

  const handleRemove = () => {
    onRemove(localItem.id);
  };

  const handleNameChange = (name: string) => {
    setLocalItem((prevItem) => ({
      ...prevItem,
      name: name,
    }));
  };

  const [showAdvanceSettings, setShowAdvanceSettings] = useState(false);
  const [isEditing, setIsEditing] = useState(false);
  const [localItem, setLocalItem] = useState(item);
  const handleEditClick = () => {
    if (isEditing) onSave(localItem);
    setIsEditing(!isEditing);
  };

  useEffect(() => {
    if (!isEditing) setLocalItem(item);
  }, [item]);

  return (
    <div
      className={`list-group-item ${
        isEditing ? "border-primary" : ""
      } d-flex flex-column m-2 border p-0 rounded`}
    >
      <ItemTitle
        title={localItem.name}
        isEditing={isEditing}
        onTextChange={handleNameChange}
        placeholder="Untitled VentDrive"
      />
      <div className="d-flex-column align-items-center w-100 mb-1 p-2">
        <label
          title="id"
          className={`form-control-plaintext text-muted w-auto`}
        >
          {"ID: " + encode64BitNumberToBase62(localItem.id)}
        </label>
        <label
          title="type"
          className={`form-control-plaintext text-muted w-auto`}
        >
          {"Type: " + localItem.type}
        </label>
        <label
          title="currentPos"
          className={`form-control-plaintext text-muted w-auto`}
        >
          {"Now Percent: " + localItem.currentVentingPercent}
        </label>
        <div className="d-flex align-items-center">
          <label
            title="currentStat"
            className={`form-control-plaintext text-muted w-auto`}
          >
            {"State: " + localItem.currentState}
          </label>
          <Spinner
            className="spinner-border mx-1"
            hidden={
              localItem.currentState === "closing" ||
              localItem.currentState === "opening"
                ? false
                : true
            }
          ></Spinner>
        </div>

        <div className="d-flex align-items-center w-100 mb-1 mx-0">
          <label className="text-muted w-auto"> Venting Percent: </label>
          <input
            title="goal percent"
            type="number"
            className={`w-auto mx-1 ${
              isEditing && !localItem.autoTempControl
                ? "form-control"
                : "form-control-plaintext text-muted"
            }`}
            value={localItem.ventingPercent}
            onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
              setLocalItem((prevItem) => ({
                ...prevItem,
                ventingPercent: Number(e.target.value),
              }));
            }}
            disabled={!isEditing && localItem.autoTempControl}
          />
        </div>
        <div className="d-flex align-items-center w-100 mb-1 mx-0">
          <label className="text-muted w-auto"> Auto Control </label>
          <div className="form-check form-switch mx-2">
            <input
              title="Auto control based on temperature"
              type="checkbox"
              className={`form-check-input`}
              checked={localItem.autoTempControl}
              onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
                setLocalItem((prevItem) => ({
                  ...prevItem,
                  autoTempControl: Boolean(e.target.checked),
                }));
              }}
              disabled={!isEditing}
            />
          </div>
        </div>
        <div className="d-flex align-items-center w-100 mb-1 mx-0">
          <label className="text-muted w-auto"> Show Advance Settings </label>
          <input
            title="Show Advance Settings"
            type="checkbox"
            className={`mx-1 form-check-input`}
            checked={showAdvanceSettings}
            onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
              setShowAdvanceSettings(e.target.checked);
            }}
          />
        </div>
        <div hidden={!showAdvanceSettings}>
          <div hidden={!localItem.autoTempControl}>
            <div>
              <SensorPicker
                devices={devices}
                isEditing={isEditing}
                selectedId={localItem.sensor}
                onChange={(selectedId: string) => {
                  setLocalItem((prevItem) => ({
                    ...prevItem,
                    sensor: selectedId,
                  }));
                }}
              />
            </div>
            <div className="d-flex align-items-center w-100 mb-1 mx-0">
              <label className="text-muted w-auto"> Temp (100%): </label>
              <input
                title="100% venting temperature"
                type="number"
                className={`w-auto mx-1 ${
                  isEditing
                    ? "form-control"
                    : "form-control-plaintext text-muted"
                }`}
                value={Number(localItem.openStateTemp)}
                onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
                  setLocalItem((prevItem) => ({
                    ...prevItem,
                    openStateTemp: Number(e.target.value),
                  }));
                }}
                disabled={!isEditing}
              />
            </div>
            <div className="d-flex align-items-center w-100 mb-1 mx-0">
              <label className="text-muted w-auto"> Temp (0%): </label>
              <input
                title="0% venting temperature"
                type="number"
                className={`w-auto mx-1 ${
                  isEditing
                    ? "form-control"
                    : "form-control-plaintext text-muted"
                }`}
                value={Number(localItem.closeStateTemp)}
                onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
                  setLocalItem((prevItem) => ({
                    ...prevItem,
                    closeStateTemp: Number(e.target.value),
                  }));
                }}
                disabled={!isEditing}
              />
            </div>
          </div>
          <div className="d-flex align-items-center w-100 mb-1 mx-0">
            <label className="text-muted w-auto"> Length: </label>
            <input
              title="length"
              type="number"
              className={`w-auto mx-1 ${
                isEditing ? "form-control" : "form-control-plaintext text-muted"
              }`}
              value={localItem.length}
              onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
                setLocalItem((prevItem) => ({
                  ...prevItem,
                  length: Number(e.target.value),
                }));
              }}
              disabled={!isEditing}
            />
          </div>
          <div className="d-flex align-items-center w-100 mb-1 mx-0">
            <label className="text-muted w-auto"> Endstop Extra mm: </label>
            <input
              title="endstopExtraDistance"
              type="number"
              className={`w-auto mx-1 ${
                isEditing ? "form-control" : "form-control-plaintext text-muted"
              }`}
              value={localItem.endstopExtraDistance}
              onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
                setLocalItem((prevItem) => ({
                  ...prevItem,
                  endstopExtraDistance: Number(e.target.value),
                }));
              }}
              disabled={!isEditing}
            />
          </div>
          <div className="d-flex align-items-center w-100 mb-1 mx-0">
            <label className="text-muted w-auto"> Step/mm: </label>
            <input
              title="stepPermm"
              type="number"
              className={`w-auto mx-1 ${
                isEditing ? "form-control" : "form-control-plaintext text-muted"
              }`}
              value={localItem.stepPermm}
              onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
                setLocalItem((prevItem) => ({
                  ...prevItem,
                  stepPermm: Number(e.target.value),
                }));
              }}
              disabled={!isEditing}
            />
          </div>
          <div className="d-flex align-items-center w-100 mb-1 mx-0">
            <label className="text-muted w-auto"> Speed: </label>
            <input
              title="speed"
              type="number"
              className={`w-auto mx-1 ${
                isEditing ? "form-control" : "form-control-plaintext text-muted"
              }`}
              value={localItem.speed}
              onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
                setLocalItem((prevItem) => ({
                  ...prevItem,
                  speed: Number(e.target.value),
                }));
              }}
              disabled={!isEditing}
            />
          </div>
          <div className="d-flex align-items-center w-100 mb-1 mx-0">
            <label className="text-muted w-auto"> Compensation: </label>
            <input
              title="maxCompensation"
              type="number"
              className={`w-auto mx-1 ${
                isEditing ? "form-control" : "form-control-plaintext text-muted"
              }`}
              value={localItem.maxCompensation}
              onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
                setLocalItem((prevItem) => ({
                  ...prevItem,
                  maxCompensation: Number(e.target.value),
                }));
              }}
              disabled={!isEditing}
            />
          </div>
          <div className="d-flex align-items-center w-100 mb-1 mx-0">
            <label className="text-muted w-auto"> Invert Direction </label>
            <input
              title="Invert Motor Direction"
              type="checkbox"
              className={`mx-1  ${
                isEditing ? "form-check-input" : "form-check-input text-muted"
              }`}
              checked={localItem.invertDir}
              onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
                setLocalItem((prevItem) => ({
                  ...prevItem,
                  invertDir: e.target.checked,
                }));
              }}
              disabled={!isEditing}
            />
          </div>
          <div className="d-flex align-items-center w-100 mb-1 mx-0">
            <label className="text-muted w-auto"> Invert Endstop Pin </label>
            <input
              title="Invert Endstop Pin State"
              type="checkbox"
              className={`mx-1 ${
                isEditing ? "form-check-input" : "form-check-input text-muted"
              }`}
              checked={localItem.invertEndstopPin}
              onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
                setLocalItem((prevItem) => ({
                  ...prevItem,
                  invertEndstopPin: e.target.checked,
                }));
              }}
              disabled={!isEditing}
            />
          </div>
        </div>

        <div className="d-flex align-items-center w-100 mb-3">
          <div className="d-flex flex-column">
            <button
              className={`btn btn-primary rounded-circle `}
              onClick={handleEditClick}
            >
              {" "}
              <div hidden={isEditing}>
                <PencilSquare />
              </div>
              <div hidden={!isEditing}>
                <FloppyFill />
              </div>
            </button>
          </div>
          <div className="d-flex align-items-center w-100 mb-1 mx-1">
            <Button
              title="autohome"
              className={`w-auto mx-1 form-control`}
              onClick={() => {
                onSave({
                  ...localItem,
                  AutoHomeFlag: true,
                });
              }}
            >
              AutoHome
            </Button>
          </div>
        </div>
        <LogConfig
          config={{
            logInterval: item.logInterval,
            logOnlyOnChange: item.logOnlyOnChange,
            loggingEnabled: item.loggingEnabled,
          }}
          onChange={(config: LogConfigProps) => {
            localItem.logInterval = config.logInterval;
            localItem.logOnlyOnChange = config.logOnlyOnChange;
            localItem.loggingEnabled = config.loggingEnabled;
          }}
          disabled={!isEditing}
        />
        <div className="d-flex w-100">
          <button
            className="btn btn-danger w-100 mx-1"
            onClick={() => {
              const isConfirmed = window.confirm("Are You Sure to Delete?");
              if (isConfirmed) {
                onRemove(localItem.id);
              }
            }}
          >
            Remove
          </button>
          <button
            className={`btn w-100 mx-1 ${
              item.status ? "btn-success" : "btn-secondary"
            }`}
            disabled
          >
            {item.status ? "Connected" : "Disconnected"}
          </button>
        </div>
      </div>
    </div>
  );
};

export default VentDriveItem;
