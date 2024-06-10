<script lang="ts">
  import { onMount, onDestroy } from 'svelte'
  import type { ROI } from '../../../types'
  export let panFactor = 1.5
  export let zoomFactor = 0.1
  let resizeObserver: ResizeObserver | null = null
  let element: HTMLDivElement | null = null
  let roi: ROI | null = null
  let imageSize: ROI | null = null
  let isPanning = false
  let startX = 0
  let startY = 0
  let loaded = false

  async function handleResize(entries: ResizeObserverEntry[]): Promise<void> {
    for (let entry of entries) {
      // Get the position relative to the browser window
      const { top, left, width, height } = entry.target.getBoundingClientRect()
      console.log(
        `Relative to browser window - Top: ${top}, Left: ${left}, Width: ${width}, Height: ${height}`
      )

      let posRoi: ROI = {
        x: left,
        y: top,
        width: width,
        height: height
      }

      if (loaded) await window.electron.ipcRenderer.invoke('SetWindowPosition', posRoi)
    }
  }

  function handleMouseDown(event: MouseEvent): void {
    if (event.ctrlKey) {
      isPanning = true
      startX = event.clientX
      startY = event.clientY
      event.preventDefault()
    }
  }

  async function handleMouseMove(event: MouseEvent): Promise<void> {
    if (isPanning && roi && imageSize) {
      const deltaX = event.clientX - startX
      const deltaY = event.clientY - startY

      let newRoi = {
        ...roi,
        x: roi.x - deltaX * panFactor,
        y: roi.y - deltaY * panFactor
      }

      newRoi.x = Math.max(0, Math.min(newRoi.x, imageSize.width - roi.width))
      newRoi.y = Math.max(0, Math.min(newRoi.y, imageSize.height - roi.height))

      roi = newRoi

      startX = event.clientX
      startY = event.clientY

      await window.electron.ipcRenderer.invoke('SetROI', roi)
    }
  }

  function handleMouseUp(): void {
    if (isPanning) {
      isPanning = false
    }
  }

  async function handleWheel(event: WheelEvent): Promise<void> {
    if (!roi || !imageSize) return
    event.preventDefault()

    const { deltaY } = event

    let newWidth = roi.width
    let newHeight = roi.height

    if (deltaY < 0) {
      // Zooming in
      newWidth -= roi.width * zoomFactor
      newHeight -= roi.height * zoomFactor
    } else {
      // Zooming out
      newWidth += roi.width * zoomFactor
      newHeight += roi.height * zoomFactor
    }

    const roiCenterX = roi.x + roi.width / 2
    const roiCenterY = roi.y + roi.height / 2

    let newRoi = {
      x: roiCenterX - newWidth / 2,
      y: roiCenterY - newHeight / 2,
      width: newWidth,
      height: newHeight
    }

    // Boundary checks
    newRoi.width = Math.max(1, Math.min(newRoi.width, imageSize.width))
    newRoi.height = Math.max(1, Math.min(newRoi.height, imageSize.height))
    newRoi.x = Math.max(0, Math.min(newRoi.x, imageSize.width - newRoi.width))
    newRoi.y = Math.max(0, Math.min(newRoi.y, imageSize.height - newRoi.height))

    roi = newRoi

    await window.electron.ipcRenderer.invoke('SetROI', roi)
  }

  onMount(async () => {
    resizeObserver = new ResizeObserver(handleResize)
    if (element) {
      resizeObserver.observe(element)
      element.addEventListener('wheel', handleWheel, { passive: false })
      element.addEventListener('mousedown', handleMouseDown)
      element.addEventListener('mousemove', handleMouseMove)
      element.addEventListener('mouseup', handleMouseUp)
    }
    await window.electron.ipcRenderer.invoke('StartStream')
    roi = await window.electron.ipcRenderer.invoke('GetROI')
    imageSize = await window.electron.ipcRenderer.invoke('GetImageSize')
    loaded = true
  })

  onDestroy(() => {
    if (resizeObserver) {
      resizeObserver.disconnect()
    }
    if (element) {
      element.removeEventListener('wheel', handleWheel)
      element.removeEventListener('mousedown', handleMouseDown)
      element.removeEventListener('mousemove', handleMouseMove)
      element.removeEventListener('mouseup', handleMouseUp)
    }
  })
</script>

<div bind:this={element} class="resizable-element" />

<style>
  .resizable-element {
    width: 100%;
    height: 100%;
    overflow: hidden;
  }
</style>
