<script lang="ts">
  import { ipcRenderer } from 'electron'

  async function startStream(): Promise<void> {
    await ipcRenderer.invoke('startStream')
  }

  async function stopStream(): Promise<void> {
    await ipcRenderer.invoke('stopStream')
  }
</script>

<div class="vertical-buttons">
  <button on:click={startStream}>Start Stream</button>
  <button on:click={stopStream}>Stop Stream</button>
</div>

<style>
  .vertical-buttons {
    display: flex;
    flex-direction: column;
    align-items: flex-start;
    margin-bottom: 20px;
    width: 100%;
    max-width: 300px;
  }

  .vertical-buttons button {
    margin-bottom: 10px;
    padding: 10px;
    font-size: 16px;
    width: 100%;
  }
</style>
