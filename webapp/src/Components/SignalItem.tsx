import React, { useState, useEffect, useMemo } from "react";
import "bootstrap/dist/css/bootstrap.min.css";
import { PencilSquare } from "react-bootstrap-icons";
import { FloppyFill } from "react-bootstrap-icons";
import ItemTitle from "../Components/ItemTitle";
import { SignalApiData } from "../api/apiService";
import SignalPicker from "./SignalPicker";
import { SignalHubItem, SignalMode } from "../api/apiService";
import { SignalType } from "../Components/SignalNameResolver";

interface SignalItemComponentProps {
  item: SignalApiData;
  onRemove: (id: number) => void;
  onSave: (item: SignalApiData) => void;
  signalHubList: SignalHubItem[];
  signalApiData?: SignalApiData[];
}

const SignalItem: React.FC<SignalItemComponentProps> = ({
  item,
  onRemove,
  onSave,
  signalHubList,
  signalApiData = [],
}) => {
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

  useEffect(() => {
    if (!isEditing) setLocalItem(item);
  }, [item]);

  const selectedBroadcaster = useMemo(
    () => [localItem.broadcaster],
    [localItem.broadcaster]
  );

  const selectedAuxiliaryBroadcaster = useMemo(
    () => [localItem.auxiliaryBroadcaster],
    [localItem.auxiliaryBroadcaster]
  );

  const handleEditClick = () => {
    if (isEditing) onSave(localItem);
    setIsEditing(!isEditing);
  };

  return (
    <div
      className={`list-group-item container-md ${
        isEditing ? "border-primary" : ""
      } d-flex flex-column align-items-start mb-3 border rounded p-0`}
    >
      <ItemTitle
        title={localItem.name}
        isEditing={isEditing}
        onTextChange={handleNameChange}
      />
      <div className="d-flex-column align-items-center w-100 p-2">
        <div className="d-flex-column align-items-center w-100 mb-1">
          <div className="d-flex align-items-center w-100 mb-1"></div>
          <div className="d-flex-collumn align-items-center w-100 mb-1">
            <SignalPicker
              signalHub={signalHubList}
              id={localItem.id}
              signalApiData={signalApiData}
              selectedValues={selectedBroadcaster}
              multiple={false}
              name="Broadcaster"
              allowedTypes={[SignalType.Broadcaster]}
              visible={isEditing}
              onChange={(selected) => {
                setLocalItem((prevItem) => ({
                  ...prevItem,
                  broadcaster: selected[0] || "",
                }));
              }}
            />
          </div>
          <div
            className={
              localItem.mode != SignalMode.SingleSource
                ? "d-flex-collumn align-items-center w-100 mb-1"
                : "d-none"
            }
          >
            <SignalPicker
              signalHub={signalHubList}
              id={localItem.id}
              signalApiData={signalApiData}
              selectedValues={selectedAuxiliaryBroadcaster}
              multiple={false}
              name="Broadcaster(Auxiliary)"
              allowedTypes={[SignalType.Broadcaster]}
              visible={isEditing}
              onChange={(selected) => {
                setLocalItem((prevItem) => ({
                  ...prevItem,
                  auxiliaryBroadcaster: selected[0] || "",
                }));
              }}
            />
          </div>
          <div className="d-flex-collumn align-items-center w-100 mb-1">
            <SignalPicker
              signalHub={signalHubList}
              id={localItem.id}
              signalApiData={signalApiData}
              selectedValues={localItem.listeners}
              multiple={true}
              name="Listeners"
              allowedTypes={[SignalType.Listener]}
              visible={isEditing}
              onChange={(selected) => {
                setLocalItem((prevItem) => ({
                  ...prevItem,
                  listeners: selected || "",
                }));
              }}
            />
          </div>
          <div className={`d-flex align-items-center w-100 mb-1`}>
            <label className={`${isEditing ? "" : "text-muted"}`}>
              {" "}
              Mode:{" "}
            </label>
            <select
              title="eventID"
              className={`w-auto mx-1 ${
                isEditing ? "form-control" : "form-control-plaintext text-muted"
              }`}
              value={localItem.mode}
              onChange={(e) => {
                setLocalItem((prevItem) => ({
                  ...prevItem,
                  mode: e.target.value as SignalMode,
                }));
              }}
              disabled={!isEditing}
            >
              {Object.values(SignalMode).map((option) => (
                <option key={option} value={option}>
                  {option}
                </option>
              ))}
            </select>
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
        </div>
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
            {item.status ? "Status: 1" : "Status: 0"}
          </button>
        </div>
      </div>
    </div>
  );
};

export default SignalItem;
