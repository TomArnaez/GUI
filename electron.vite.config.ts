import { defineConfig, externalizeDepsPlugin } from 'electron-vite'
import { svelte } from '@sveltejs/vite-plugin-svelte'
import renderer from 'vite-plugin-electron-renderer'

export default defineConfig({
  main: {
    plugins: [externalizeDepsPlugin()]
  },
  preload: {
    plugins: [externalizeDepsPlugin()]
  },
  renderer: {
    plugins: [
      svelte(),
      renderer({
        nodeIntegration: true,
        optimizeDeps: {
          include: [{ name: 'gpu_viewport', type: 'commonjs' }]
        }
      })
    ]
  }
})
