function handleRowClick(event) {
    // Obtém o conteúdo da linha clicada
    const conteudoLinha = event.currentTarget.textContent;

    // Exibe um alerta com o conteúdo da linha clicada
    alert(`Você clicou na linha com o conteúdo: ${conteudoLinha}`);
}

// Adiciona um evento de clique a todas as linhas da tabela
const linhas = document.querySelectorAll('tr');
linhas.forEach((linha) => {
    linha.addEventListener('click', handleRowClick);
});