export type ROI = {
  x: number
  y: number
  width: number
  height: number
}

export class GPUViewport {
  constructor(windowHandle: number)
  public initDetector(): number
  public initRenderer(): number
  public configureStream(expTime: number): number
  public startStream(): number
  public stopStream(): number
  public getROI(): ROI
  public setROI(roi: ROI): number
  public getImageSize(): ROI
  public setWindowPosition(roi: ROI)
  public setHistogramEqualisation(enabled: boolean): number
  public setDarkCorrection(darkMapPath: string): number
  public setGainCorrection(defectMapPath: string): number
  public setDefectCorrection(defectMapPath: string): number
  public cleanup(): null
}
