import React from "react";
import SchedulerItem from "./SchedulerItem";
import "bootstrap/dist/css/bootstrap.min.css";
import { SchedulerItemProps } from "./SchedulerItem";

interface SchedulerListGroupProps {
  items: SchedulerItemProps[];
  onCreate: (item: SchedulerItemProps) => void;
  onSave: (item: SchedulerItemProps) => void;
  onRemove: (id: number) => void;
  currentTime: string;
  currentWeekday: string;
}

const SchedulerListGroup: React.FC<SchedulerListGroupProps> = ({
  items,
  onSave,
  onCreate,
  onRemove,
  currentTime,
  currentWeekday,
}) => {
  const handleAddItem = () => {
    const newItem: SchedulerItemProps = {
      id: nextId(),
      name: "Untitled Schedule",
      weekday: "1100000",
      start: "00:00",
      duration: "01:30",
      enabled: true,
      status: false,
      mode: "weekly",
      skipped: false,
    };
    onCreate(newItem);
  };

  const handleRemoveItem = (id: number) => {
    onRemove(id);
  };

  function nextId() {
    let newId = 1;
    while (items.some((item) => item.id === newId)) {
      newId++;
    }
    return newId;
  }

  const handleSave = (item: SchedulerItemProps) => {
    onSave(item);
  };

  const getWeekdayName = (weekday: string) => {
    let index = 0;
    let foundWeekday: string = "Unknown";
    if (weekday) {
      const days = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];
      for (let i = 0; i < 7; i++) {
        if (weekday[i] === "1") index = i;
      }
      foundWeekday = days[index];
    }
    return foundWeekday;
  };

  return (
    <div
      className="d-flex flex-column align-items-center mb-3 border p-2 rounded p-2 bg-light"
      style={{ width: "fit-content", height: "fit-content" }}
    >
      <div className="d-flex align-items-center w-100">
        <label className="text-muted w-auto mx-2"> Time: {currentTime}</label>
        <label className="text-muted w-auto mx-2">
          {" "}
          Weekday: {getWeekdayName(currentWeekday)}
        </label>
        <label className="text-muted w-auto mx-2"> Date: </label>
      </div>
      <div
        className="list-group overflow-auto"
        style={{ maxHeight: "400px", width: "fit-content" }}
      >
        {items.map((item) => (
          <SchedulerItem
            key={item.id}
            item={item}
            onRemove={(id) => handleRemoveItem(id)}
            onSave={(updatedItem) => handleSave(updatedItem)}
          />
        ))}
      </div>
      <div className="d-flex align-items-center w-100">
        <button className="btn btn-primary m-4 w-100" onClick={handleAddItem}>
          Add New Schedule
        </button>
      </div>
    </div>
  );
};

export default SchedulerListGroup;
