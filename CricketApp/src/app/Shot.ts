export class Shot {
  id: number;
  shotCount: number;
  name: string;
  force: number = -1;
  velocity: number = -1;
  displacement: number = -1;
}

export class Scale {
  ball: string;
  rw1: number;
  rw2: number;
  rw3: number;
  sum: number;
  w1x: number;
  w1y: number;
  w2x: number;
  w2y: number;
  w3x: number;
  w3y: number;
  deltaX: number;
  deltaY: number;
  distX: number;
  distY: number;
  sampleNo: number;
}