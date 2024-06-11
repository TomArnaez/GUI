<script lang="ts">
  import { ipcRenderer } from 'electron'

  async function startStream(): Promise<void> {
    await ipcRenderer.invoke('startStream')
  }
</script>

<div class="vertical-buttons">
  <button on:click={startStream}>Start Stream</button>
</div>

<style>
  .vertical-buttons {
    display: flex;
    flex-direction: column;
    align-items: flex-start;
  }

  .vertical-buttons button {
    margin-bottom: 10px;
    padding: 10px;
    font-size: 16px;
  }
</style>
