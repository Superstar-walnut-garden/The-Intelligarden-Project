import React, { useState } from "react";
import WeekSelector from "./WeekSelector";
import "bootstrap/dist/css/bootstrap.min.css";
import { PencilSquare } from "react-bootstrap-icons";
import { FloppyFill } from "react-bootstrap-icons";
import ItemTitle from "./ItemTitle";

export interface SchedulerItemProps {
  id: number;
  weekday: string;
  start: string;
  duration: string;
  enabled: boolean;
  status: boolean;
  name: string;
  mode: string;
  skipped: boolean;
}

interface SchedulerItemComponentProps {
  item: SchedulerItemProps;
  onRemove: (id: number) => void;
  onSave: (item: SchedulerItemProps) => void;
}

const SchedulerItem: React.FC<SchedulerItemComponentProps> = ({
  item,
  onRemove,
  onSave,
}) => {
  const handleWeekSelectorChange = (weekday: string) => {
    setLocalItem((prevItem) => ({
      ...prevItem,
      weekday: weekday,
    }));
  };
  const handleRemove = () => {
    onRemove(localItem.id);
  };

  const handleStartTimeChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    setLocalItem((prevItem) => ({
      ...prevItem,
      start: e.target.value,
    }));
  };

  const handleDurationChange = (duration: string) => {
    setLocalItem((prevItem) => ({
      ...prevItem,
      duration: duration,
    }));
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
      mode: String(e.target.value),
    }));
  };

  const [isEditing, setIsEditing] = useState(false);
  const [localItem, setLocalItem] = useState(item);
  const handleEditClick = () => {
    if (isEditing) {
      if (
        localItem.duration === "" ||
        /^([01]\d|2[0-3]):([0-5]\d)$/.test(localItem.duration)
      ) {
        onSave(localItem);
        setIsEditing(false);
      } else
        alert(
          "invalid time format in 'Duration' field! Please enter in a format like 02:45"
        );
    } else setIsEditing(true);
  };

  return (
    <div
      className={`list-group-item ${
        isEditing ? "border-primary" : ""
      } d-flex flex-column align-items-start m-2 border p-0 rounded`}
    >
      <ItemTitle
        title={localItem.name}
        isEditing={isEditing}
        onTextChange={handleNameChange}
      />
      <div className="d-flex flex-column align-items-center w-100 mb-3">
        <div className="d-flex align-items-center w-100 mb-1">
          <label className="text-muted w-auto mx-2"> ID: {item.id}</label>
        </div>
        <div className="d-flex align-items-center w-100 mb-1">
          <label className="text-muted w-auto mx-2"> Mode: </label>
          <select
            title="mode"
            className={`w-auto mx-1 ${
              isEditing ? "form-control" : "form-control-plaintext text-muted"
            }`}
            value={localItem.mode}
            onChange={handleModeChange}
            disabled={!isEditing}
          >
            <option value={"monthly"} disabled>
              Monthly(not implemented)
            </option>
            <option value={"weekly"}>Weekly</option>
            <option value={"daily"} disabled>
              Daily(not implemented)
            </option>
            <option value={"hourly"}>Hourly</option>
          </select>
        </div>
      </div>

      <div className="d-flex align-items-center w-100 mb-3">
        <div className="d-flex flex-column mx-3">
          <label>{localItem.mode === "weekly" ? "Start:" : "Interval"} </label>
          <input
            title="Start"
            type="time"
            className={`${
              isEditing ? "form-control" : "form-control-plaintext text-muted"
            }`}
            value={localItem.start}
            onChange={handleStartTimeChange}
            disabled={!isEditing}
          />
        </div>
        <div className="d-flex justify-content-end flex-column mx-3">
          <label>Duration: </label>
          <input
            title="Duration"
            type="input"
            maxLength={5}
            size={5}
            placeholder="HH:MM"
            className={`flex-shrink w-auto ${
              isEditing ? "form-control" : "form-control-plaintext text-muted"
            }`}
            value={localItem.duration}
            onChange={(e) => {
              const value = e.target.value;
              //if (value === "" || /^([01]\d|2[0-3]):([0-5]\d)$/.test(value))
              handleDurationChange(value);
            }}
            disabled={!isEditing}
          />
        </div>
        <div className="d-flex flex-column m-2">
          <button
            className={`btn btn-primary rounded-circle `}
            onClick={handleEditClick}
          >
            <div hidden={isEditing}>
              <PencilSquare />
            </div>
            <div hidden={!isEditing}>
              <FloppyFill />
            </div>
          </button>
        </div>
      </div>
      {localItem.mode === "weekly" && (
        <div className="d-flex flex-column align-items-start mb-3 ">
          <label className="mx-3"> Week Days: </label>
          <WeekSelector
            initialSelectedDays={item.weekday}
            onSelectionChange={handleWeekSelectorChange}
            isEnabled={isEditing}
          />
        </div>
      )}
      <div className="d-flex w-100 p-1">
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
            item.enabled ? "btn-secondary" : "btn-success"
          }`}
          // onClick={onToggleEnable}
        >
          {item.enabled ? "Disable" : "Enable"}
        </button>
      </div>
    </div>
  );
};

export default SchedulerItem;
