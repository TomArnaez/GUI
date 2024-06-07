<script lang="ts">
  import { onMount, onDestroy } from 'svelte'
  import { createEventDispatcher } from 'svelte'

  export let width: number
  export let height: number

  let element: HTMLDivElement | null = null
  const dispatch = createEventDispatcher()

  function handleResize(entries: ResizeObserverEntry[]): void {
    for (let entry of entries) {
      width = entry.contentRect.width
      height = entry.contentRect.height
      dispatch('resize', { width, height })
    }
  }

  function handleWheel(event: WheelEvent): void {
    event.preventDefault()
    if (event.deltaY > 0) {
      width = Math.max(100, width - 10)
      height = Math.max(100, height - 10)
    } else {
      width = Math.min(800, width + 10)
      height = Math.min(800, height + 10)
    }
    if (element) {
      element.style.width = `${width}px`
      element.style.height = `${height}px`
    }
  }

  let resizeObserver: ResizeObserver | null = null

  onMount(() => {
    resizeObserver = new ResizeObserver(handleResize)
    if (element) {
      resizeObserver.observe(element)
      element.addEventListener('wheel', handleWheel, { passive: false })
    }
  })

  onDestroy(() => {
    if (resizeObserver) {
      resizeObserver.disconnect()
    }
    if (element) {
      element.removeEventListener('wheel', handleWheel)
    }
  })
</script>

<div bind:this={element} class="resizable-element" style="width: {width}px; height: {height}px;">
  <p>Resize me with the mouse wheel</p>
  <p>Current width: {width}px</p>
  <p>Current height: {height}px</p>
</div>

<style>
  .resizable-element {
    border: 1px solid #ccc;
    padding: 10px;
    overflow: hidden;
    display: flex;
    justify-content: center;
    align-items: center;
    box-sizing: border-box;
  }
</style>
