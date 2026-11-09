import React, { useEffect, useState } from "react";
import TitleBar from "../Components/TitleBar";
import * as ApiService from "../api/apiService";

const DisplaySettings: React.FC = () => {
  const [config, setConfig] = useState<ApiService.DisplayConfig>({
    type: "oled",
    address: 60,
  });
  useEffect(() => {
    const getDisplayConfig = async () => {
      setConfig(await ApiService.getDisplayConfig());
    };
    getDisplayConfig();
  }, []);

  const handleTypeChange = (e: React.ChangeEvent<HTMLSelectElement>) => {
    setConfig((prevItem) => ({
      ...prevItem,
      type: String(e.target.value),
    }));
  };

  const handleSave = () => {
    ApiService.setDisplayConfig(config);
  };

  return (
    <>
      <TitleBar title="Display Settings" />
      <div className="container mt-4 bg-light p-4 rounded border w-50">
        <label>Display Type:</label>
        <select
          className="form-control mb-2"
          title="displayType"
          value={config.type}
          onChange={handleTypeChange}
        >
          <option>None</option>
          <option value={"Oled"}>OLED (I2C)</option>
          <option value={"CharLcd"}>Char LCD (I2C)</option>
          <option disabled>1bit GLCD (Parallel)</option>
          <option disabled>TFT LCD HMI</option>
        </select>
        <label>Display Size:</label>
        <select className="form-control mb-2" title="displayType">
          <option>128x64</option>
          <option>2x16</option>
          <option>4x20</option>
        </select>
        <label>I2C Address:</label>
        <input
          type="text"
          className="form-control mb-4"
          title="i2cAddress"
          value={config.address || ""}
          onChange={(e) =>
            setConfig((prev) => ({
              ...prev,
              address: Number(e.target.value),
            }))
          }
        />
        <button
          className="btn btn-primary w-auto mx-auto d-block"
          onClick={handleSave}
        >
          Save
        </button>
      </div>
    </>
  );
};

export default DisplaySettings;
