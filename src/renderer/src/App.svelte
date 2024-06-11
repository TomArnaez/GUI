<script lang="ts">
  import DetectorControl from './components/DetectorControl.svelte'
  import Viewport from './components/Viewport.svelte'
  import Database from "better-sqlite3";

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
    height: 100vh; /* Make the container take the full height of the viewport */
  }

  .button-list {
    width: 200px; /* Fixed width for the button list column */
    background-color: #f0f0f0; /* Optional: background color for the button list */
    padding: 1rem;
  }

  .viewport {
    flex-grow: 1; /* Make the viewport take the remaining space */
    background-color: #fff; /* Optional: background color for the viewport */
    padding: 1rem;
  }
</style>
