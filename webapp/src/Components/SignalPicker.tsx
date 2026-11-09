import React, { useEffect, useState } from "react";

import {
  SignalApiData,
  SignalEndpoint,
  SignalHubItem,
} from "../api/apiService";
import {
  SignalNameResolver,
  SignalType,
} from "../Components/SignalNameResolver";

interface SignalPickerProps {
  signalHub: SignalHubItem[];
  id: number;
  signalApiData?: SignalApiData[];
  selectedValues?: SignalEndpoint[];
  multiple?: boolean;
  onChange?: (selected: SignalEndpoint[]) => void;
  visible?: boolean;
  name?: string;
  allowedTypes?: SignalType[];
}

type DrillLevel =
  | { type: "subsystem"; name: string }
  | { type: "item"; subsystem: string; name: string };

const SignalPicker: React.FC<SignalPickerProps> = ({
  signalHub,
  id,
  signalApiData,
  selectedValues = [],
  multiple = false,
  onChange,
  visible = true,
  name = "signals",
  allowedTypes = [SignalType.Broadcaster, SignalType.Listener],
}) => {
  const [selected, setSelected] = useState<SignalEndpoint[]>(selectedValues);
  const [path, setPath] = useState<DrillLevel[]>([]);
  const [modalOpen, setModalOpen] = useState(false);

  useEffect(() => {
    onChange?.(selected);
  }, [selected]);

  useEffect(() => {
    setSelected(selectedValues);
  }, [selectedValues]);

  const handleBack = () => {
    setPath((prev) => prev.slice(0, -1));
  };

  const getItemNameForSignal = (signal: string): string | undefined => {
    for (const subsystem of signalHub) {
      for (const item of subsystem.items) {
        if (item.signals.includes(signal)) {
          return item.name;
        }
      }
    }
    return undefined;
  };

  const isSignalLocked = (signal: string): boolean => {
    if (!signalApiData) return false;
    for (const entry of signalApiData) {
      if (
        entry.listeners.some((l) => l.signalPath === signal) &&
        entry.id !== id
      ) {
        return true;
      }
    }
    return false;
  };

  const toggleSelection = (signal: string) => {
    const existing = selected.find((s) => s.signalPath === signal);
    const updatedSelection = multiple
      ? existing
        ? selected.filter((s) => s.signalPath !== signal)
        : [...selected, { signalPath: signal, inverted: false, status: false }]
      : [{ signalPath: signal, inverted: false, status: false }];

    setSelected(updatedSelection);
  };

  const toggleInversion = (signal: string, value: boolean) => {
    const updated = selected.map((s) =>
      s.signalPath === signal ? { ...s, inverted: value } : s
    );
    setSelected(updated);
  };

  const getSignalTypeLabel = (value: SignalType): string =>
    Object.entries(SignalType).find(([, v]) => v === value)?.[0] ?? value;

  const renderLevel = () => {
    if (path.length === 0) {
      return (
        <ul className="list-group">
          {signalHub.map((sub) => (
            <li
              key={sub.name}
              className="list-group-item list-group-item-action"
              onClick={() => setPath([{ type: "subsystem", name: sub.name }])}
              style={{ cursor: "pointer" }}
            >
              {sub.name}
            </li>
          ))}
        </ul>
      );
    }

    if (path.length === 1 && path[0].type === "subsystem") {
      const subsystem = signalHub.find((s) => s.name === path[0].name);
      if (!subsystem) return null;

      return (
        <ul className="list-group">
          {subsystem.items.map((item) => (
            <li
              key={item.name}
              className="list-group-item list-group-item-action"
              onClick={() =>
                setPath([
                  { type: "subsystem", name: subsystem.name },
                  { type: "item", subsystem: subsystem.name, name: item.name },
                ])
              }
              style={{ cursor: "pointer" }}
            >
              {item.name}
            </li>
          ))}
        </ul>
      );
    }

    if (
      path.length === 2 &&
      path[0].type === "subsystem" &&
      path[1].type === "item"
    ) {
      const subsystem = signalHub.find((s) => s.name === path[0].name);
      const item = subsystem?.items.find((i) => i.name === path[1].name);
      if (!item) return null;

      return (
        <ul className="list-group">
          {item.signals.map((signal) => {
            const parsed = SignalNameResolver.parse(signal);
            const type = parsed.type;
            const isAllowed = allowedTypes.includes(type);
            const isSelected = selected.some((s) => s.signalPath === signal);
            const isLocked =
              type === SignalType.Listener && isSignalLocked(signal);

            const handleClick = () => {
              if (!isAllowed) {
                alert(
                  `Only ${allowedTypes
                    .map(getSignalTypeLabel)
                    .join(" / ")} signals can be selected.`
                );
              } else if (isLocked) {
                alert(
                  `This Listener is already connected to another broadcaster and cannot be selected.`
                );
              } else {
                toggleSelection(signal);
              }
            };

            const inversionCheckbox = isSelected && (
              <div className="d-flex align-items-center rounded border mx-2">
                <label className="form-check-label m-2">Invert ↶</label>
                <input
                  type="checkbox"
                  checked={
                    selected.find((s) => s.signalPath === signal)?.inverted ??
                    false
                  }
                  onClick={(e) => e.stopPropagation()}
                  onChange={(e) => toggleInversion(signal, e.target.checked)}
                  className="form-check-input me-2"
                  title="Invert signal"
                />
              </div>
            );

            return (
              <li
                key={signal}
                className={`list-group-item list-group-item-action d-flex justify-content-between align-items-center ${
                  isAllowed && !isLocked ? "" : "text-muted"
                }`}
                onClick={handleClick}
                style={{ cursor: "pointer" }}
              >
                <div className="d-flex align-items-center">
                  <span>
                    {isSelected
                      ? `✅ ${parsed.localSignalName}`
                      : parsed.localSignalName}
                  </span>
                  {inversionCheckbox}
                </div>
              </li>
            );
          })}
        </ul>
      );
    }

    return null;
  };

  return (
    <div className="p-2 border rounded" style={{ maxWidth: "600px" }}>
      <div className="d-flex align-items-center gap-2">
        {!visible && (
          <label className={`${visible ? "" : "text-muted"}`}>{name}:</label>
        )}
        {visible && (
          <button
            className="btn btn-sm btn-outline-primary"
            onClick={() => setModalOpen(true)}
          >
            Edit {name}
          </button>
        )}
      </div>

      <ul className="list-group p-0">
        {selected.map((signal) => {
          const parsed = SignalNameResolver.parse(signal.signalPath);
          const itemName = getItemNameForSignal(signal.signalPath);
          return (
            parsed.localSignalName.length > 0 &&
            parsed.subsystemName.length > 0 && (
              <li
                key={signal.signalPath}
                className="list-group-item border-0 mx-0 px-0"
              >
                <div className="d-flex">
                  <span
                    className={`px-1 justify-content-center text-center ${
                      signal.status ? "text-success" : "text-warning"
                    }`}
                  >
                    {signal.status ? "⦿" : "⦾"}
                  </span>
                  <div className="d-flex flex-column">
                    <div className="d-flex">
                      <span className="lh-sm">{itemName}</span>
                      {!visible && (
                        <span className="lh-sm px-1">
                          {signal.inverted ? " ↶" : ""}{" "}
                        </span>
                      )}
                      {visible && (
                        <span
                          className="lh-sm mx-1 text-primary"
                          style={{ cursor: "pointer" }}
                          title="Toggle inversion"
                          onClick={() => {
                            setSelected((prev) =>
                              prev.map((s) =>
                                s.signalPath === signal.signalPath
                                  ? { ...s, inverted: !s.inverted }
                                  : s
                              )
                            );
                          }}
                        >
                          {signal.inverted ? "↶" : "↑"}
                        </span>
                      )}
                      {visible && (
                        <span
                          className="lh-sm mx-1 text-danger"
                          style={{ cursor: "pointer" }}
                          title="Remove signal"
                          onClick={() => {
                            setSelected((prev) =>
                              prev.filter(
                                (s) => s.signalPath !== signal.signalPath
                              )
                            );
                          }}
                        >
                          Delete
                        </span>
                      )}
                    </div>

                    <small className="text-muted lh-1">
                      ({parsed.subsystemName}→{parsed.localSignalName})
                    </small>
                  </div>
                </div>
              </li>
            )
          );
        })}
      </ul>

      {modalOpen && (
        <div
          className="modal d-block"
          tabIndex={-1}
          role="dialog"
          style={{ backgroundColor: "rgba(0,0,0,0.5)" }}
        >
          <div className="modal-dialog modal-dialog-scrollable" role="document">
            <div className="modal-content">
              <div className="modal-header">
                <h5 className="modal-title">Pick Signals</h5>
                <button
                  type="button"
                  className="btn-close"
                  onClick={() => {
                    setModalOpen(false);
                    setPath([]);
                  }}
                ></button>
              </div>
              <div className="modal-body">
                {path.length > 0 && (
                  <button
                    className="btn btn-sm btn-outline-secondary mb-2"
                    onClick={handleBack}
                  >
                    ← Back
                  </button>
                )}
                {renderLevel()}
              </div>
            </div>
          </div>
        </div>
      )}
    </div>
  );
};

export default SignalPicker;
