import React, { useState, useEffect } from "react";
import SignalListGroup from "../Components/SignalListGroup";
import { SignalApiData } from "../api/apiService";
import * as ApiService from "../api/apiService";
import TitleBar from "../Components/TitleBar";

const SignalManager: React.FC = () => {
  const [signals, setSignals] = useState<SignalApiData[]>([]);
  const [signalhubList, setSignalhubList] = useState<
    ApiService.SignalHubItem[]
  >([]);

  useEffect(() => {
    const interval = setInterval(() => {
      getList();
      getSignalHub();
    }, 2000);
    return () => clearInterval(interval);
  }, []);

  const getList = async () => {
    try {
      const result = await ApiService.getSignalList();
      setSignals(result);
    } catch (error) {
      console.error("Error fetching signal list:", error);
      alert("Error fetching signal list:" + error);
    }
  };
  const getSignalHub = async () => {
    try {
      const result = await ApiService.getSignalHubList();
      setSignalhubList(result);
    } catch (error) {
      console.error("Error fetching signal list:", error);
      alert("Error fetching signal list:" + error);
    }
  };

  const createSignal = async (signal: SignalApiData) => {
    try {
      ApiService.createSignal(signal);
      getList();
    } catch (error) {
      console.error("Error creating signal:", error);
      alert("Error creating signal:" + error);
    }
  };

  const deleteSignal = async (id: number) => {
    try {
      await ApiService.deleteSignal(id);
      getList();
    } catch (error) {
      console.error("Error deleting signal:", error);
      alert("Error deleting signal:" + error);
    }
  };

  const modifySignal = async (signal: SignalApiData) => {
    try {
      await ApiService.modifySignal(signal.id, signal);
      getList();
    } catch (error) {
      console.error("Error deleting signal:", error);
      alert("Error creating signal:" + error);
    }
  };

  return (
    <>
      <TitleBar title="Unified Signal-Hub" />
      <div className="d-flex flex-column align-items-center">
        <SignalListGroup
          items={signals}
          onCreate={createSignal}
          onRemove={deleteSignal}
          onSave={modifySignal}
          signalHubList={signalhubList}
          signalApiData={signals}
        />
      </div>
    </>
  );
};

export default SignalManager;
