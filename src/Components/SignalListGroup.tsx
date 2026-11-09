import React from "react";
import "bootstrap/dist/css/bootstrap.min.css";
import { SignalApiData, SignalHubItem, SignalMode } from "../api/apiService";
import SignalItem from "../Components/SignalItem";

interface SignalListGroupProps {
  items: SignalApiData[];
  signalHubList: SignalHubItem[];
  signalApiData?: SignalApiData[]; // optional but recommended
  onCreate: (item: SignalApiData) => void;
  onSave: (item: SignalApiData) => void;
  onRemove: (id: number) => void;
}

const SignalListGroup: React.FC<SignalListGroupProps> = ({
  items,
  onSave,
  onCreate,
  onRemove,
  signalHubList,
  signalApiData = [],
}) => {
  const handleAddItem = () => {
    const newItem: SignalApiData = {
      id: nextId(),
      name: "Untitled Signal",
      status: false,
      mode: SignalMode.SingleSource,
      broadcaster: { signalPath: "", status: false, inverted: false },
      auxiliaryBroadcaster: { signalPath: "", status: false, inverted: false },
      listeners: [],
    };
    onCreate(newItem);
  };

  function nextId() {
    let newId = 1;
    while (items.some((item) => item.id === newId)) {
      newId++;
    }
    return newId;
  }

  const handleRemoveItem = (id: number) => {
    onRemove(id);
  };

  const handleSave = (item: SignalApiData) => {
    onSave(item);
  };

  return (
    <div
      className="d-flex flex-column align-items-center mb-3 border p-2 rounded p-2"
      style={{ width: "fit-content", height: "fit-content" }}
    >
      <div
        className="list-group overflow-auto"
        style={{ maxHeight: "65vh", width: "40vh" }}
      >
        <div className="px-3">
          {items.map((item) => (
            <SignalItem
              key={item.id}
              item={item}
              onRemove={(id) => handleRemoveItem(id)}
              onSave={(updatedItem) => handleSave(updatedItem)}
              signalHubList={signalHubList}
              signalApiData={signalApiData}
            />
          ))}
        </div>
      </div>
      <div className="d-flex align-items-center w-100">
        <button className="btn btn-primary m-4 w-100" onClick={handleAddItem}>
          New Signal Router
        </button>
      </div>
    </div>
  );
};

export default SignalListGroup;
