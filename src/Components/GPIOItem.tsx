import React, { useState } from "react";
import "bootstrap/dist/css/bootstrap.min.css";
import { PencilSquare } from "react-bootstrap-icons";
import { FloppyFill } from "react-bootstrap-icons";
import ItemTitle from "./ItemTitle";
import LogConfig from "./LogConfig";
import { LogConfigProps } from "./LogConfig";

export interface GPIOItemProps {
  id: number;
  mode: number;
  event_id: number;
  status: boolean;
  name: string;
  logInterval: number;
  logOnlyOnChange: boolean;
  loggingEnabled: boolean;
  highDutyCycle?: number;
  inverted?: boolean;
}

interface GPIOItemComponentProps {
  item: GPIOItemProps;
  onRemove: (pin: number) => void;
  onSave: (item: GPIOItemProps) => void;
}

const GPIOItem: React.FC<GPIOItemComponentProps> = ({
  item,
  onRemove,
  onSave,
}) => {
  const handleRemove = () => {
    onRemove(localItem.id);
  };

  const handleNameChange = (text: string) => {
    setLocalItem((prevItem) => ({
      ...prevItem,
      name: text,
    }));
  };

  const handleModeChange = (e: React.ChangeEvent<HTMLSelectElement>) => {
    setLocalItem((prevItem) => ({
      ...prevItem,
      mode: Number(e.target.value),
    }));
  };

  const handleDutyChange = (e: React.ChangeEvent<HTMLSelectElement>) => {
    setLocalItem((prevItem) => ({
      ...prevItem,
      highDutyCycle: Number(e.target.value),
    }));
  };

  const handlePinChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    setLocalItem((prevItem) => ({
      ...prevItem,
      id: Number(e.target.value),
    }));
  };

  const [isEditing, setIsEditing] = useState(false);
  const [localItem, setLocalItem] = useState(item);
  const handleEditClick = () => {
    if (isEditing) onSave(localItem);
    setIsEditing(!isEditing);
  };

  return (
    <div
      className={`list-group-item ${
        isEditing ? "border-primary" : ""
      } d-flex flex-column align-items-start m-2 border rounded p-0`}
    >
      <ItemTitle
        title={localItem.name}
        isEditing={isEditing}
        onTextChange={handleNameChange}
      />
      <div className="d-flex flex-column align-items-center w-100 m-2 mb-3">
        <div className="d-flex align-items-center w-100 mb-1">
          <label className="text-muted w-auto"> Pin: </label>
          <input
            title="pin"
            type="text"
            className={`w-auto mx-1 ${
              isEditing ? "form-control" : "form-control-plaintext text-muted"
            }`}
            value={localItem.id}
            onChange={handlePinChange}
            disabled={!isEditing}
          />
        </div>
        <div className="d-flex align-items-center w-100 mb-1">
          <label className="text-muted w-auto"> Mode: </label>
          <select
            title="mode"
            className={`w-auto mx-1 ${
              isEditing ? "form-control" : "form-control-plaintext text-muted"
            }`}
            value={localItem.mode}
            onChange={handleModeChange}
            disabled={!isEditing}
          >
            <option value={0}>Input</option>
            <option value={1}>Output</option>
          </select>
        </div>
        <div className="d-flex align-items-center w-100 mb-1">
          <label className="text-muted w-auto"> High Dutycycle: </label>
          <select
            title="high dutycycle"
            className={`w-auto mx-1 ${
              isEditing ? "form-control" : "form-control-plaintext text-muted"
            }`}
            value={localItem.highDutyCycle}
            onChange={handleDutyChange}
            disabled={!isEditing}
          >
            <option value={10}>10%</option>
            <option value={20}>20%</option>
            <option value={30}>30%</option>
            <option value={40}>40%</option>
            <option value={50}>50%</option>
            <option value={60}>60%</option>
            <option value={70}>70%</option>
            <option value={80}>80%</option>
            <option value={90}>90%</option>
            <option value={100}>100%</option>
          </select>
        </div>
        {/* inversion checkbox */}
        <div className="d-flex align-items-center w-100 mb-1">
          <label
            className="form-check-label text-muted"
            htmlFor={`inverted-${localItem.id}`}
          >
            Invert
          </label>
          <input
            className="form-check-input mx-1"
            type="checkbox"
            id={`inverted-${localItem.id}`}
            checked={localItem.inverted || false}
            onChange={(e) =>
              setLocalItem((prevItem) => ({
                ...prevItem,
                inverted: e.target.checked,
              }))
            }
            disabled={!isEditing}
          />
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

      <div className="d-flex align-items-center w-100 m-2 mb-3">
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
      </div>
      <div className="d-flex w-100 mb-1">
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
          onClick={() => {
            localItem.status = !localItem.status;
            onSave(localItem);
          }}
        >
          {item.status ? "I/O: High" : "I/O: Low"}
        </button>
      </div>
    </div>
  );
};

export default GPIOItem;
