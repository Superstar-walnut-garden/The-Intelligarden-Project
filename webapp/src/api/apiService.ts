import axios from "axios";
import { SchedulerItemProps } from "../Components/SchedulerItem";
import { GPIOItemProps } from "../Components/GPIOItem";

const apiBaseUrl = "/api";

// -----------------------
// Log File Manager API
// -----------------------

export interface LogFileContent {
  [timestamp: string]: Record<string, any>;
}

// GET handler for a single file
export const getLogFile = async (path: string): Promise<LogFileContent> => {
  const response = await axios.get<LogFileContent>(
    `/api/file?path=${encodeURIComponent(path)}`
  );
  return response.data;
};

// Define types for clarity
export interface LogFile {
  name: string;
  path: string;
  size: number;
}

export interface LogFolder {
  name: string;
  path: string;
  folders: LogFolder[];
  files: LogFile[];
}

export interface LogFilesResponse {
  path: string;
  folders: LogFolder[];
  files: LogFile[];
}

// API handler
export const getLogFiles = async (): Promise<LogFilesResponse> => {
  const response = await axios.get<LogFilesResponse>(apiBaseUrl + "/files");
  return response.data;
};

// -----------------------
// Logging API
// -----------------------
export enum LogDispatcherStatus {
  Idle = "Idle",
  Running = "Running",
  StorageFullError = "StorageFullError",
  StorageNotReadyError = "StorageNotReadyError",
}
export interface DataLoggingConfigApiData {
  maxFileSizeBytes: number;
  maxFileRotationCount: number;
  basePath: string;
  status: LogDispatcherStatus;
  enabled: boolean;
}

export const getDataLoggingConfig =
  async (): Promise<DataLoggingConfigApiData> => {
    const response = await axios.get<DataLoggingConfigApiData>(
      apiBaseUrl + "/log-config"
    );
    return response.data;
  };

export const setDataLoggingConfig = async (
  payload: DataLoggingConfigApiData
): Promise<void> => {
  await axios.put(apiBaseUrl + "/log-config", payload, {
    headers: { "Content-Type": "application/json" },
  });
};

// -----------------------
// Signal API
// -----------------------
export interface SignalEndpoint {
  signalPath: string;
  inverted: boolean;
  status: boolean;
}
export enum SignalMode {
  SingleSource = "SingleSource",
  AndWithAuxiliary = "AndWithAuxiliary",
  OrWithAuxiliary = "OrWithAuxiliary",
}
export interface SignalApiData {
  id: number;
  name: string;
  status: boolean;
  mode: SignalMode;
  broadcaster: SignalEndpoint;
  auxiliaryBroadcaster: SignalEndpoint;
  listeners: SignalEndpoint[];
}
export const getSignalList = async (): Promise<SignalApiData[]> => {
  const response = await axios.get<SignalApiData[]>(apiBaseUrl + "/signal");
  return response.data;
};

export const createSignal = async (newSignal: SignalApiData): Promise<void> => {
  await axios.post(apiBaseUrl + "/signal", newSignal);
};

export const deleteSignal = async (id: number): Promise<void> => {
  await axios.delete(apiBaseUrl + "/signal?id=" + id);
};

export const modifySignal = async (
  id: number,
  signal: SignalApiData
): Promise<void> => {
  await axios.put(apiBaseUrl + "/signal?id=" + id, { ...signal });
};

// -----------------------
// Firebase API
// -----------------------
export interface FirebaseSettingsData {
  apiKey: string;
  databaseURL: string;
  userEmail: string;
  userPassword: string;
  databaseRootName: string;
  enabled: boolean;
}

export const getFirebaseData = async (): Promise<FirebaseSettingsData> => {
  const response = await axios.get<FirebaseSettingsData>(
    apiBaseUrl + "/firebase-config"
  );
  return response.data;
};

export const setFirebaseData = async (
  payload: FirebaseSettingsData
): Promise<void> => {
  await axios.put(apiBaseUrl + "/firebase-config", payload, {
    headers: { "Content-Type": "application/json" },
  });
};

// -----------------------
// Wifi API
// -----------------------
export interface WifiState {
  on: boolean;
  ssid: string;
  password: string;
  dhcpEnabled: boolean;
  ip?: string;
  subnet?: string;
  gateway?: string;
  primaryDNS?: string;
  secondaryDNS?: string;
}

export const getWifiState = async (): Promise<WifiState> => {
  const response = await axios.get<WifiState>(apiBaseUrl + "/wifi-config");
  return response.data;
};

export const setWifiConfig = async (payload: WifiState): Promise<void> => {
  await axios.put(apiBaseUrl + "/wifi-config", payload, {
    headers: { "Content-Type": "application/json" },
  });
};

// -----------------------
// Hotspot API
// -----------------------
export interface HotspotConfig {
  ssid: string;
  password: string;
}

export const getHotspotConfig = async (): Promise<HotspotConfig> => {
  const response = await axios.get<HotspotConfig>(
    apiBaseUrl + "/hotspot-config"
  );
  return response.data;
};

export const setHotspotConfig = async (
  payload: HotspotConfig
): Promise<void> => {
  await axios.put(apiBaseUrl + "/hotspot-config", payload, {
    headers: { "Content-Type": "application/json" },
  });
};

// -----------------------
// temp sensor config API
// -----------------------
export interface TempSensorConfig {
  sensorPin: number;
}

export const getTempSensorConfig = async (): Promise<TempSensorConfig> => {
  const response = await axios.get<TempSensorConfig>(
    apiBaseUrl + "/temp-sensor-config"
  );
  return response.data;
};

export const setTempSensorConfig = async (
  payload: TempSensorConfig
): Promise<void> => {
  await axios.put(apiBaseUrl + "/temp-sensor-config", payload, {
    headers: { "Content-Type": "application/json" },
  });
};

// -----------------------
// Schedule API
// -----------------------
export const getScheduleList = async (): Promise<SchedulerItemProps[]> => {
  const response = await axios.get<SchedulerItemProps[]>(
    apiBaseUrl + "/scheduler"
  );
  return response.data;
};

export const createSchedule = async (
  item: SchedulerItemProps
): Promise<void> => {
  await axios.post(apiBaseUrl + "/scheduler", item);
};

export const deleteSchedule = async (id: number): Promise<void> => {
  await axios.delete(apiBaseUrl + "/scheduler?id=" + id);
};

export const modifySchedule = async (
  id: number,
  updatedItem: Partial<SchedulerItemProps>
): Promise<void> => {
  await axios.put(apiBaseUrl + "/scheduler?id=" + id, updatedItem);
};

// -----------------------
// Current Time API
// -----------------------
export interface CurrentTimeData {
  time: string;
  weekday: string;
}

export const getCurrentTime = async (): Promise<CurrentTimeData> => {
  const response = await axios.get<CurrentTimeData>(
    apiBaseUrl + "/getCurrentTime"
  );
  return response.data;
};

// -----------------------
// Sensor API
// -----------------------
export interface FusionBusApiData {
  type: string;
  id: number;
  name: string;
  status: boolean;
  logInterval: number;
  logOnlyOnChange: boolean;
  loggingEnabled: boolean;
}
export interface TempSensorApiData extends FusionBusApiData {
  temp: number;
}
export interface VentDriveApiData extends FusionBusApiData {
  ventingPercent: number;
  length: number;
  stepPermm: number;
  speed: number;
  maxCompensation: number;
  acceleration: number;
  endstopExtraDistance: number;
  currentState: string;
  currentVentingPercent: number;
  AutoHomeFlag: boolean;
  invertDir: boolean;
  invertEndstopPin: boolean;
  autoTempControl: boolean;
  closeStateTemp: Number;
  openStateTemp: Number;
  sensor: string;
}

export const getFusionBusList = async (): Promise<any> => {
  const response = await axios.get(apiBaseUrl + "/fusionbus");
  return response.data;
};

export const deleteFusionBusItem = async (id: number): Promise<void> => {
  await axios.delete(apiBaseUrl + "/fusionbus?id=" + id);
};

export const modifyFusionBusItem = async (
  id: number,
  item: TempSensorApiData | VentDriveApiData
): Promise<void> => {
  await axios.put(apiBaseUrl + "/fusionbus?id=" + id, item);
};

// -----------------------
// GPIO API
// -----------------------
export const getGPIOList = async (): Promise<GPIOItemProps[]> => {
  const response = await axios.get<GPIOItemProps[]>(apiBaseUrl + "/gpio");
  return response.data;
};

export const createGPIO = async (item: GPIOItemProps): Promise<void> => {
  await axios.post(apiBaseUrl + "/gpio", item);
};

export const deleteGPIO = async (id: number): Promise<void> => {
  await axios.delete(apiBaseUrl + "/gpio?id=" + id);
};

export const modifyGPIO = async (
  id: number,
  item: GPIOItemProps
): Promise<void> => {
  await axios.put(apiBaseUrl + "/gpio?id=" + id, item);
};

// -----------------------
// Display API
// -----------------------
export interface DisplayConfig {
  type: string;
  address: number;
}

export const getDisplayConfig = async (): Promise<DisplayConfig> => {
  const response = await axios.get<DisplayConfig>(
    apiBaseUrl + "/display-config"
  );
  return response.data;
};

export const setDisplayConfig = async (
  config: DisplayConfig
): Promise<void> => {
  await axios.put(apiBaseUrl + "/display-config", config);
};

// -----------------------
// Thermostat API
// -----------------------
export interface ThermostatApiData {
  id: number;
  event_id: number;
  heaterEvent_id: number;
  coolerEvent_id: number;
  sensor: string;
  name: string;
  setpoint: number;
  altSetpoint: number;
  hysteresis: number;
  offset: number;
  status: boolean;
  enabled: boolean;
}

export const getThermostatList = async (): Promise<ThermostatApiData[]> => {
  const response = await axios.get<ThermostatApiData[]>(
    apiBaseUrl + "/thermostat"
  );
  return response.data;
};

export const createThermostate = async (
  item: ThermostatApiData
): Promise<void> => {
  await axios.post(apiBaseUrl + "/thermostat", item);
};

export const deleteThermostat = async (id: number): Promise<void> => {
  await axios.delete(apiBaseUrl + "/thermostat?id=" + id);
};

export const modifyThermostat = async (
  id: number,
  item: ThermostatApiData
): Promise<void> => {
  await axios.put(apiBaseUrl + "/thermostat?id=" + id, item);
};

// -----------------------
// System API
// -----------------------
export const rebootSystem = async (): Promise<void> => {
  await axios.post(apiBaseUrl + "/restart", {});
};

// -----------------------
// Hotspot API
// -----------------------
export interface TimeConfigApiData {
  timezone: string;
  ntpServer: string;
  timeSubsystemInitialized: boolean;
  externalRTCAvailable: boolean;
  setTimeAutomatically: boolean;
  ntpUpdated: boolean;
  manualTimeSetFlag: boolean;
  manualTimeEpoch: number;
}

export const getTimeConfig = async (): Promise<TimeConfigApiData> => {
  const response = await axios.get<TimeConfigApiData>(
    apiBaseUrl + "/time-config"
  );
  return response.data;
};

export const setTimeConfig = async (
  payload: TimeConfigApiData
): Promise<void> => {
  await axios.put(apiBaseUrl + "/time-config", payload, {
    headers: { "Content-Type": "application/json" },
  });
};

// -----------------------
// Signal-Hub API
// -----------------------
export interface SignalHubItem {
  name: string;
  items: {
    name: string;
    signals: string[];
  }[];
}

export const getSignalHubList = async (): Promise<SignalHubItem[]> => {
  const response = await axios.get<SignalHubItem[]>(apiBaseUrl + "/signal-hub");
  return response.data;
};
