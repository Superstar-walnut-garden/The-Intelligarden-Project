import React from "react";
import GPIOItem from "./GPIOItem";
import "bootstrap/dist/css/bootstrap.min.css";
import { GPIOItemProps } from "./GPIOItem";

interface GPIOListGroupProps {
  items: GPIOItemProps[];
  onCreate: (item: GPIOItemProps) => void;
  onSave: (item: GPIOItemProps) => void;
  onRemove: (id: number) => void;
}

const GPIOListGroup: React.FC<GPIOListGroupProps> = ({
  items,
  onSave,
  onCreate,
  onRemove,
}) => {
  const handleAddItem = () => {
    const input = window.prompt("Enter pin number:");
    if (input !== null) {
      const pin = Number(input);
      if (pin >= 0) {
        const newItem: GPIOItemProps = {
          id: pin,
          mode: 1, // default mode is output!
          status: false,
          name: "Untitled IO",
          event_id: -1,
          loggingEnabled: false,
          logInterval: 60,
          logOnlyOnChange: false,
        };
        onCreate(newItem);
      } else {
        alert("Please enter a valid positive pin number.");
      }
    }
  };

  const handleRemoveItem = (id: number) => {
    onRemove(id);
  };

  const handleSave = (item: GPIOItemProps) => {
    onSave(item);
  };

  return (
    <div
      className="d-flex flex-column align-items-center mb-3 border p-2 rounded p-2"
      style={{ width: "fit-content", height: "fit-content" }}
    >
      <div
        className="list-group overflow-auto"
        style={{ maxHeight: "400px", width: "fit-content" }}
      >
        {items.map((item) => (
          <GPIOItem
            key={item.id}
            item={item}
            onRemove={(id) => handleRemoveItem(id)}
            onSave={(updatedItem) => handleSave(updatedItem)}
          />
        ))}
      </div>
      <div className="d-flex align-items-center w-100">
        <button className="btn btn-primary m-4 w-100" onClick={handleAddItem}>
          Config New Pin
        </button>
      </div>
    </div>
  );
};

export default GPIOListGroup;
