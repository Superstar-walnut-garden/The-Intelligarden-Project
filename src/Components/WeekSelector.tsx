import React, { useState, useEffect, useRef } from "react";
import "bootstrap/dist/css/bootstrap.min.css";
import "./WeekSelector.css";

interface WeekSelectorProps {
  isEnabled?: boolean;
  initialSelectedDays?: string;
  onSelectionChange?: (selectedDays: string) => void;
}

const daysOfWeek = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];

const WeekSelector: React.FC<WeekSelectorProps> = ({
  initialSelectedDays = "0000000",
  onSelectionChange,
  isEnabled = false,
}) => {
  const [selectedDays, setSelectedDays] = useState<boolean[]>(
    initialSelectedDays.split("").map((char) => char === "1")
  );

  const prevSelectedDays = useRef<string>(initialSelectedDays);

  useEffect(() => {
    const currentSelectedDays = getSelectedDaysString();
    if (onSelectionChange && prevSelectedDays.current !== currentSelectedDays) {
      onSelectionChange(currentSelectedDays);
      prevSelectedDays.current = currentSelectedDays;
    }
  }, [selectedDays, onSelectionChange]);

  const toggleDay = (index: number) => {
    if (isEnabled) {
      const newSelectedDays = [...selectedDays];
      newSelectedDays[index] = !newSelectedDays[index];
      setSelectedDays(newSelectedDays);
    }
  };

  const getSelectedDaysString = () => {
    return selectedDays.map((day) => (day ? "1" : "0")).join("");
  };

  return (
    <div className="d-flex align-items-center border m-2 rounded-pill">
      {daysOfWeek.map((day, index) => (
        <div
          style={{ borderRadius: "50%" }}
          key={index}
          className={`btn btn-circle ${
            selectedDays[index] ? "btn-success" : "btn-outline-secondary"
          } m-1`}
          onClick={() => toggleDay(index)}
        >
          <p className="d-flex justify-content-center align-items-center">
            {day}
          </p>
        </div>
      ))}
    </div>
  );
};

export default WeekSelector;
