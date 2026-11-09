import React, { useEffect, useState } from "react";
import TitleBar from "../Components/TitleBar";
import * as ApiService from "../api/apiService";
import BadgeIndicator from "../Components/BadgeIndicator";
import DateTimePicker from "../Components/DateTimePicker";

const TimezoneSettings: React.FC = () => {
  const [timeConfig, setTimeConfig] = useState<ApiService.TimeConfigApiData>({
    timezone: "",
    ntpServer: "",
    manualTimeSetFlag: false,
    manualTimeEpoch: Math.floor(Date.now() / 1000), // Default to current time
    timeSubsystemInitialized: false,
    externalRTCAvailable: false,
    setTimeAutomatically: true,
    ntpUpdated: false,
  });

  const fetchTimeConfig = async () => {
    try {
      const data = await ApiService.getTimeConfig();
      setTimeConfig(data);
    } catch (error) {
      console.error("Error fetching time configuration:", error);
    }
  };

  useEffect(() => {
    fetchTimeConfig();
  }, []);

  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const { name, value } = e.target;
    setTimeConfig((prev) => ({
      ...prev,
      [name]: name === "manualTimeEpoch" ? Number(value) : value,
    }));
  };

  const handleCheckboxChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const { name, checked } = e.target;
    setTimeConfig((prev) => ({
      ...prev,
      [name]: checked,
    }));
  };

  const handleDateTimeChange = (epoch: number) => {
    setTimeConfig((prev) => ({
      ...prev,
      manualTimeEpoch: epoch,
    }));
  };

  const handleSave = async () => {
    try {
      await ApiService.setTimeConfig(timeConfig);
      fetchTimeConfig(); // Refresh the configuration after saving
      alert("Time configuration saved successfully!");
    } catch (error) {
      console.error("Error saving time configuration:", error);
      alert("Failed to save time configuration.");
    }
  };

  return (
    <>
      <TitleBar title="Date & Time Settings" />
      <div className="container mt-4">
        <div className="card p-4">
          <h3 className="card-title">Configure Date & Time</h3>
          <div className="form-check form-switch mt-3">
            <input
              className="form-check-input"
              type="checkbox"
              id="setTimeAutomatically"
              name="setTimeAutomatically"
              checked={timeConfig.setTimeAutomatically}
              onChange={handleCheckboxChange}
            />
            <label className="form-check-label" htmlFor="setTimeAutomatically">
              Set Time Automatically
            </label>
          </div>
          {timeConfig.setTimeAutomatically && (
            <div className="form-group">
              <div className="form-group">
                <label htmlFor="ntpServer">NTP Server</label>
                <input
                  type="text"
                  id="ntpServer"
                  name="ntpServer"
                  className="form-control"
                  value={timeConfig.ntpServer}
                  onChange={handleInputChange}
                  placeholder="e.g., pool.ntp.org"
                />
              </div>
              <BadgeIndicator
                label="Synced with Server"
                status={timeConfig.ntpUpdated}
                successText="OK"
                failureText="NO"
              />
            </div>
          )}
          {!timeConfig.setTimeAutomatically && (
            <div className="form-group">
              <div className="d-flex-collum align-items-center">
                <DateTimePicker
                  onDateTimeChange={handleDateTimeChange}
                  initialEpoch={Math.floor(
                    (Date.now() - new Date().getTimezoneOffset() * 60000) / 1000
                  )}
                  onSet={() => {
                    const newState = { ...timeConfig, manualTimeSetFlag: true };
                    ApiService.setTimeConfig(newState);
                    fetchTimeConfig(); // Refresh the configuration after setting time
                  }}
                />
              </div>
            </div>
          )}
          <div className="form-group">
            <label htmlFor="timezone">Timezone</label>
            <input
              type="text"
              id="timezone"
              name="timezone"
              className="form-control"
              value={timeConfig.timezone}
              onChange={handleInputChange}
              placeholder="e.g. PST8PDT,M3.2.0/2,M11.1.0/2, UTC0"
            />
          </div>
          <BadgeIndicator
            label="External RTC"
            status={timeConfig.externalRTCAvailable}
            successText="OK"
            failureText="Not Found!"
          />
          <BadgeIndicator
            label="Time Subsystem"
            status={timeConfig.timeSubsystemInitialized}
            successText="OK"
            failureText="Not Initialized!"
          />
          <button className="btn btn-primary mt-3" onClick={handleSave}>
            Save Configuration
          </button>
        </div>
      </div>
    </>
  );
};

export default TimezoneSettings;
