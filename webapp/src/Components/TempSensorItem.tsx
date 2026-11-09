import React, { useState, useEffect } from "react";
import "bootstrap/dist/css/bootstrap.min.css";
import { PencilSquare } from "react-bootstrap-icons";
import { FloppyFill } from "react-bootstrap-icons";
import { TempSensorApiData } from "../api/apiService";
import ItemTitle from "../Components/ItemTitle";
import LogConfig from "./LogConfig";
import { LogConfigProps } from "./LogConfig";

interface TempSensorItemComponentProps {
  item: TempSensorApiData;
  onRemove: (id: number) => void;
  onSave: (item: TempSensorApiData) => void;
}

const TempSensorItem: React.FC<TempSensorItemComponentProps> = ({
  item,
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
        placeholder="Untitled Sensor"
      />
      <div className="d-flex-column align-items-center w-100 mb-1 p-2">
        <label
          title="id"
          className={`form-control-plaintext text-muted w-auto`}
        >
          {"Sensor ID: " + encode64BitNumberToBase62(localItem.id)}
        </label>
        <div className="d-flex align-items-center w-100 mb-1">
          <label
            title="id"
            className={`form-control-plaintext text-muted w-auto`}
          >
            {"Temp: " + localItem.temp + "C"}
          </label>
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

export default TempSensorItem;
