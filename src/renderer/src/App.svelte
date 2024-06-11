<script lang="ts">
  import DetectorControl from './components/DetectorControl.svelte'
  import Viewport from './components/Viewport.svelte'
</script>

<div class="container">
  <div class="button-list">
    <DetectorControl />
  </div>
  <div class="viewport">
    <Viewport />
  </div>
</div>

<style>
  .container {
    display: flex;
    height: 100vh;
  }

  .button-list {
    width: 200px;
    background-color: #f0f0f0;
    padding: 1rem;
  }

  .viewport {
    flex-grow: 1;
    background-color: #fff;
    padding: 1rem;
  }
</style>
