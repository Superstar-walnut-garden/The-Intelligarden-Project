import React, { useEffect, useState } from "react";

import { FusionBusApiData } from "../api/apiService";

interface SensorPickerProps {
  devices: FusionBusApiData[];
  selectedId: string;
  onChange: (selectedId: string) => void;
  isEditing: boolean;
}

const SensorPicker: React.FC<SensorPickerProps> = ({
  devices,
  selectedId,
  onChange,
  isEditing,
}) => {
  return (
    <div className="d-flex align-items-center w-100 mb-0">
      <label
        title="sensor"
        className={`form-control-plaintext text-muted w-auto`}
      >
        Sensor:
      </label>
      <select
        title="SensorListDropdown"
        value={selectedId}
        onChange={(e: React.ChangeEvent<HTMLSelectElement>) => {
          onChange(e.target.value);
        }}
        disabled={!isEditing}
        className={`w-auto mx-1 mb-0 ${
          !isEditing ? "form-control-plaintext text-muted" : "form-control"
        }`}
      >
        <option value={-1}>No sensors assigned!</option>
        {devices.map(
          (sensor) =>
            sensor.type === "TempSensor" &&
            sensor.name && (
              <option key={sensor.id} value={sensor.id}>
                {sensor.name}
              </option>
            )
        )}
      </select>
    </div>
  );
};

export default SensorPicker;
