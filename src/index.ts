import * as fs from "fs-extra";

// eslint-disable-next-line @typescript-eslint/no-var-requires
const addon = require("bindings")("yrp.node");
const NativeReplay: { new (buffer: Buffer): NativeReplay } = addon.Replay;

interface NativeReplay {
    getHeaderInformation(): ReplayHeader;
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

export class Replay {
    public static async fromFile(path: string) {
        const buffer = await fs.readFile(path);
        const nativeReplay = new NativeReplay(buffer);

        return new Replay(nativeReplay);
    }
    public static async fromBuffer(buffer: Buffer) {
        return new Replay(new NativeReplay(buffer));
    }

    private readonly native: NativeReplay;
    private replayHeader: ReplayHeader | null;

    private constructor(nativeReplay: NativeReplay) {
        this.native = nativeReplay;
        this.replayHeader = null;
    }

    public getReplayHeader() {
        if (this.replayHeader) {
            this.replayHeader = this.native.getHeaderInformation();
        }

        return { ...this.replayHeader };
    }
}
