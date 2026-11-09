export enum SignalType {
  Broadcaster = "B",
  Listener = "L",
}

export interface SignalNameParameters {
  subsystemName: string;
  id: number;
  localSignalName: string;
  type: SignalType;
}

export class SignalNameResolver {
  static toString(p: SignalNameParameters): string {
    return `${p.subsystemName}_${p.id}_${p.localSignalName}`;
    // localSignalName already includes type suffix (_B or _L)
  }

  static parse(signalPath: string): SignalNameParameters {
    const firstDelim = signalPath.indexOf("_");
    const secondDelim = signalPath.indexOf("_", firstDelim + 1);
    const thirdDelim = signalPath.indexOf("_", secondDelim + 1);

    const subsystemName = signalPath.substring(0, firstDelim);
    const idStr = signalPath.substring(firstDelim + 1, secondDelim);
    const localSignalName = signalPath.substring(secondDelim + 1, thirdDelim);
    const typeSuffix = signalPath.substring(thirdDelim + 1);

    return {
      subsystemName,
      id: parseInt(idStr, 10),
      localSignalName,
      type: this.parseType(typeSuffix),
    };
  }

  static parseType(signalPath: string): SignalType {
    const lastChar = signalPath.charAt(signalPath.length - 1);
    return lastChar === "B" ? SignalType.Broadcaster : SignalType.Listener;
  }

  static generateLocalSignalName(
    locallyUniqueName: string,
    type: SignalType
  ): string {
    return `${locallyUniqueName}_${this.signalTypeToString(type)}`;
  }

  private static signalTypeToString(type: SignalType): string {
    return type === SignalType.Broadcaster ? "B" : "L";
  }

  private static parseSignalType(type: string): SignalType {
    return type === "B" ? SignalType.Broadcaster : SignalType.Listener;
  }
}
