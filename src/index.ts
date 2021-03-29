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
    private readonly header: ReplayHeader;
    private readonly parameter: ReplayParameter;
    private readonly scriptName: string;
    private readonly playerNames: string[];

    private constructor(nativeReplay: NativeReplay) {
        this.native = nativeReplay;
        this.header = this.native.getHeaderInformation();
        this.parameter = this.native.getParameters();
        this.playerNames = this.native.getPlayerNames();
        this.scriptName = this.native.getScriptName();
    }

    public getHeader() {
        return { ...this.header };
    }
    public getPlayerNames() {
        return [...this.playerNames];
    }
    public getParameter() {
        return { ...this.parameter };
    }
    public getScriptName() {
        return this.scriptName;
    }
}
