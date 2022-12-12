# ABCg

Daniel Fusimoto Pires - RA: 11201921874

![linux workflow](https://github.com/hbatagelo/abcg/actions/workflows/linux.yml/badge.svg)
![macOS workflow](https://github.com/hbatagelo/abcg/actions/workflows/macos.yml/badge.svg)
![Windows workflow](https://github.com/hbatagelo/abcg/actions/workflows/windows.yml/badge.svg)
![WASM workflow](https://github.com/hbatagelo/abcg/actions/workflows/wasm.yml/badge.svg)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/hbatagelo/abcg)](https://github.com/hbatagelo/abcg/releases/latest)

Development framework accompanying the course [MCTA008-17 Computer Graphics](http://professor.ufabc.edu.br/~harlen.batagelo/cg/) at [UFABC](https://www.ufabc.edu.br/).

[Documentation](https://hbatagelo.github.io/abcg/abcg/doc/html/) \| [Release notes](CHANGELOG.md) 

ABCg is a lightweight C++ framework that simplifies the development of 3D graphics applications based on [OpenGL](https://www.opengl.org), [OpenGL ES](https://www.khronos.org), [WebGL](https://www.khronos.org/webgl/), and [Vulkan](https://www.vulkan.org). It is designed for the tutorials and assignments of the course "MCTA008-17 Computer Graphics" taught at Federal University of ABC (UFABC).

# Atividade 04 - Simulação de jogo de plataforma em primeira pessoa

## Descrição Geral

- Foi desenvolvido um ambiente simples de blocos espalhados, onde o usuário pode se mover pulando de bloco em bloco.
- O projeto foi baseado na movimentação do jogo Minecraft, neste projeto foram implementadas as funções de movimentação do personagem e da câmera, gravidade, salto, agachamento e corrida. Além um sistema simples de colisão que permite detectar se o usuário errou ou não oo pulo.

## To Do / Não deu tempo para fazer

- Por falta de tempo não consegui iniciar o processo de texturização dos blocos, iluminação mais elaborada e texturização do ambiente.
- Também não consegui gerar os cubos por mim mesmo, ao invés de importar um .obj, acabei tendo problemas no processo de montar os triângulos.

## Implementação

### `main.cpp, lookat.frag, lookat.vert`
- Estes 3 arquivos foram reutilizados do projeto LookAt
- main.cpp é responsável pela inicialização do programa/janela da aplicação.
- lookat.frag, lookat.vert são responsáveis pelo shaders da aplicação

### `trackball.hpp e trackball.cpp`
- Aqui foi utilizada de base a implementação de trackball das atividades Viewer, entretanto, de uma forma mais simplificada. Aqui apenas lê-se o posicionamento do mouse da janela da aplicação e o armazena na variável m_lastXY, que é acessavel publicamente pela função getXY.

``` cpp
void TrackBall::mouseMove(glm::ivec2 const &position) {
  m_lastXY = updateXY(position);
}

void TrackBall::resizeViewport(glm::ivec2 const &size) {
  m_viewportSize = size;
}

glm::vec3 TrackBall::updateXY(glm::vec2 const &position) const {
  return glm::vec3(
      2.0f * position.x / gsl::narrow<float>(m_viewportSize.x) - 1.0f,
      1.0f - 2.0f * position.y / gsl::narrow<float>(m_viewportSize.y), 0.0f);
}

glm::vec3 TrackBall::getXY() const {
  return m_lastXY;
}
```
- Essa simplificação se deu porque para a movimentação da câmera no lookAt já implementado, apenas as coordenadas X e Y já eram suficientes, uma matriz com componentes de rotação X, Y e Z gerariam problemas posteriormente na responsividade e intuitividade da câmera.

Controles

W A S D -> Movimentação 
Mouse -> Câmera (semelhante ao controle trackball, segurar botão esquerdo e arrastar na tela)
Shift Esquerdo -> Faz personagem esgueirar e andar mais devagar
Control Esquerdo -> Faz personagem correr quando combinado com WASD
Barra de Espaço -> Pulo

## License

ABCg is licensed under the MIT License. See [LICENSE](https://github.com/hbatagelo/abcg/blob/main/LICENSE) for more information.
