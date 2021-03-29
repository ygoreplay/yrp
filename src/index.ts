import * as fs from "fs-extra";

// eslint-disable-next-line @typescript-eslint/no-var-requires
const addon = require("bindings")("yrp.node");
const NativeReplay: { new (buffer: Buffer): NativeReplay } = addon.Replay;

interface NativeReplay {
    getHeaderInformation(): ReplayHeader;
    getPlayerNames(): string[];
    getParameters(): ReplayParameter;
    getScriptName(): string;
}

export interface ReplayHeader {
    id: number;
    version: number;
    flag: number;
    seed: number;
    dataSize: number;
    hash: number;
    props: Buffer;
}

export interface ReplayParameter {
    startLP: number;
    startHand: number;
    drawCount: number;
    duelFlags: number;
}

export class Replay {
    public static async fromFile(path: string) {
        const buffer = await fs.readFile(path);
        const nativeReplay = new NativeReplay(buffer);

        return new Replay(nativeReplay);
    }
    public static fromBuffer(buffer: Buffer) {
        return new Replay(new NativeReplay(buffer));
    }

    private readonly native: NativeReplay;
    private readonly replayHeader: ReplayHeader;
    private readonly replayParameter: ReplayParameter;
    private readonly replayScriptName: string;
    private readonly playerNames: string[];

    private constructor(nativeReplay: NativeReplay) {
        this.native = nativeReplay;
        this.replayHeader = this.native.getHeaderInformation();
        this.replayParameter = this.native.getParameters();
        this.playerNames = this.native.getPlayerNames();
        this.replayScriptName = this.native.getScriptName();
    }

    public getReplayHeader() {
        return { ...this.replayHeader };
    }
    public getPlayerNames() {
        return [...this.playerNames];
    }
    public getReplayParameter() {
        return { ...this.replayParameter };
    }
    public getReplayScriptName() {
        return this.replayScriptName;
    }
}
