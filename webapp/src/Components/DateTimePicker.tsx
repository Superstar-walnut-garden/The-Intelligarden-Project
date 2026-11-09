import React, { useState, useEffect } from "react";

interface DateTimePickerProps {
  onSet: () => void; // Callback to handle the set button click
  onDateTimeChange: (epoch: number) => void; // Callback to return the epoch timestamp
  initialEpoch?: number; // Optional initial epoch value
}

const DateTimePicker: React.FC<DateTimePickerProps> = ({
  onDateTimeChange,
  onSet,
  initialEpoch = Math.floor(Date.now() / 1000), // Default to current time
}) => {
  const initialDate = new Date(initialEpoch * 1000);

  const [dateTime, setDateTime] = useState({
    year: initialDate.getUTCFullYear(),
    month: initialDate.getUTCMonth() + 1, // Months are 0-indexed in JavaScript
    day: initialDate.getUTCDate(),
    hour: initialDate.getUTCHours(),
    minute: initialDate.getUTCMinutes(),
  });

  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const { name, value } = e.target;
    setDateTime((prev) => ({
      ...prev,
      [name]: Number(value),
    }));
  };

  const handleChange = () => {
    const { year, month, day, hour, minute } = dateTime;
    const utcDate = new Date(Date.UTC(year, month - 1, day, hour, minute)); // Create UTC date
    const epoch = Math.floor(utcDate.getTime() / 1000); // Convert to seconds
    onDateTimeChange(epoch); // Pass the epoch to the parent component
  };

  const handleSet = () => {
    onSet();
  };
  // Automatically call handleChange whenever dateTime changes
  useEffect(() => {
    handleChange();
  }, [dateTime]); // Dependency array ensures this runs whenever dateTime changes

  return (
    <div className="date-time-picker p-3 border rounded d-inline-block">
      <label htmlFor="year" className="form-label">
        Year
      </label>
      <input
        type="number"
        id="year"
        name="year"
        className="form-control"
        value={dateTime.year}
        onChange={handleInputChange}
      />

      <label htmlFor="month" className="form-label">
        Month
      </label>
      <input
        type="number"
        id="month"
        name="month"
        className="form-control"
        value={dateTime.month}
        onChange={handleInputChange}
        min={1}
        max={12}
      />

      <label htmlFor="day" className="form-label">
        Day
      </label>
      <input
        type="number"
        id="day"
        name="day"
        className="form-control"
        value={dateTime.day}
        onChange={handleInputChange}
        min={1}
        max={31}
      />
      <label htmlFor="hour" className="form-label">
        Hour
      </label>
      <input
        type="number"
        id="hour"
        name="hour"
        className="form-control"
        value={dateTime.hour}
        onChange={handleInputChange}
        min={0}
        max={23}
      />

      <label htmlFor="minute" className="form-label">
        Minute
      </label>
      <input
        type="number"
        id="minute"
        name="minute"
        className="form-control"
        value={dateTime.minute}
        onChange={handleInputChange}
        min={0}
        max={59}
      />
      <button className="btn btn-primary mt-3" onClick={handleSet}>
        Set Time
      </button>
    </div>
  );
};

export default DateTimePicker;
