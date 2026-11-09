import React, { useState, useEffect } from "react";
import SchedulerListGroup from "../Components/SchedulerListGroup";
import TitleBar from "../Components/TitleBar";
import { SchedulerItemProps } from "../Components/SchedulerItem";
import * as ApiService from "../api/apiService";

const PumpSettings: React.FC = () => {
  const [schedulerItems, setSchedulerItems] = useState<SchedulerItemProps[]>(
    []
  );
  const [currentTime, setCurrentTime] = useState<string>("");
  const [currentWeekday, setCurrentWeekday] = useState<string>("");
  useEffect(() => {
    fetchScheduleList();
    fetchCurrentTime();
    const intervalId = setInterval(fetchCurrentTime, 60000); // Update every minute

    return () => clearInterval(intervalId);
  }, []);

  const fetchScheduleList = async () => {
    try {
      const response = await ApiService.getScheduleList();
      setSchedulerItems(response);
    } catch (error) {
      console.error("Error fetching schedule list:", error);
    }
  };

  const fetchCurrentTime = async () => {
    try {
      const response = await ApiService.getCurrentTime();
      setCurrentTime(response.time);
      setCurrentWeekday(response.weekday);
    } catch (error) {
      console.error("Failed to fetch current time:", error);
    }
  };

  const createSchedule = async (item: SchedulerItemProps) => {
    const newSchedule = { ...item };
    try {
      await ApiService.createSchedule(newSchedule);
    } catch (error) {
      console.error("Error creating schedule:", error);
    }
    fetchScheduleList(); //get updated schedule list
  };

  const deleteSchedule = async (id: number) => {
    try {
      await ApiService.deleteSchedule(id);
    } catch (error) {
      console.error("Error deleting schedule:", error);
    }
    fetchScheduleList(); //get updated schedule list
  };

  const modifySchedule = async (
    id: number,
    updatedItem: Partial<SchedulerItemProps>
  ) => {
    try {
      await ApiService.modifySchedule(id, updatedItem);
      setSchedulerItems(
        schedulerItems.map((item) =>
          item.id === id ? { ...item, ...updatedItem } : item
        )
      );
    } catch (error) {
      console.error("Error modifying schedule:", error);
    }
    fetchScheduleList(); // get updated schedule list
  };

  const handleSave = (item: SchedulerItemProps) => {
    modifySchedule(item.id, item);
    console.log("Modified schedules saved!");
  };

  return (
    <>
      <div className="flex-d align-items-center">
        <TitleBar title="Manage Schedulers" />
        <div className="d-flex justify-content-center align-items-center">
          <SchedulerListGroup
            items={schedulerItems}
            onSave={handleSave}
            onRemove={deleteSchedule}
            onCreate={createSchedule}
            currentTime={currentTime}
            currentWeekday={currentWeekday}
          />
        </div>
      </div>
    </>
  );
};

export default PumpSettings;
